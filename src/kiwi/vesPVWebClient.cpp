/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

#include "vesPVWebClient.h"
#include "vesPVWebDataSet.h"

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cassert>

#include <curl/curl.h>
#include <cJSON.h>

namespace {

jsonSharedPtr makeShared(cJSON* json)
{
  return jsonSharedPtr(json, free);
}

void strPrint(const std::string& title, const std::string& str)
{
  printf("----%s----\n%s\n-----\n", title.c_str(), str.c_str());
}

size_t download_dataset(char *inBuffer, size_t size, size_t nmemb, void *userData)
{
  vesPVWebDataSet* dataset = static_cast<vesPVWebDataSet*>(userData);

  size_t totalSize = size*nmemb;
  size_t requiredSize = dataset->m_writePosition + totalSize;


  if (dataset->m_bufferSize < requiredSize) {
    dataset->m_buffer = static_cast<char*>(realloc(dataset->m_buffer, requiredSize));
    assert(dataset->m_buffer);
    dataset->m_bufferSize = requiredSize;
  }

  memcpy(&(dataset->m_buffer[dataset->m_writePosition]), inBuffer, totalSize);
  dataset->m_writePosition += totalSize;

  return totalSize;
}

std::string jsonToStr(cJSON* json)
{
  if (json) {
    char* cstr = cJSON_Print(json);
    std::string str = cstr;
    free(cstr);
    return str;
  }
  else {
    return "(null json object)";
  }
}

std::string jsonToStr(jsonSharedPtr json)
{
  return jsonToStr(json.get());
}

cJSON* make_json(int id, const std::string& method, cJSON* params)
{
  cJSON* newJson;
  newJson = cJSON_CreateObject();
  cJSON_AddNumberToObject(newJson, "id", id);
  cJSON_AddStringToObject(newJson, "method", method.c_str());
  cJSON_AddItemToObject(newJson, "params", params);
  return newJson;
}

size_t write_data(char *buffer, size_t size, size_t nmemb, void *userData)
{
  size_t totalSize = size*nmemb;
  std::stringstream& resp = *static_cast<std::stringstream*>(userData);
  resp << std::string(buffer, totalSize);
  return totalSize;
}

}




vesPVWebClient::vesPVWebClient()
{
  this->m_id = 1;
  this->m_curl = curl_easy_init();
  if (!this->m_curl) {
    std::cout << "error initializing CURL object" << std::endl;
    this->setError("cURL Error", "There was an error initializing cURL.");
  }
}

vesPVWebClient::~vesPVWebClient()
{
  curl_easy_cleanup(this->m_curl);
}

void vesPVWebClient::downloadObjects()
{
  for (size_t i = 0; i < m_datasets.size(); ++i) {
    //printf("downloading dataset %zu\n", i);
    if (!this->downloadObject(i)) {
      this->setError("Problem Downloading Geometry", "An error occured wihle downloading geometry from ParaView Web");
      return;
    }
  }
}

bool vesPVWebClient::downloadObject(int objectIndex)
{
  if (m_sessionId.empty()) {
    //std::cout << "downloadObject: session id is not initialized" << std::endl;
    return false;
  }

  if (m_viewId.empty()) {
    //std::cout << "downloadObject: view id is not initialized" << std::endl;
    return false;
  }

  if (objectIndex < 0 || objectIndex >= m_datasets.size()) {
    //std::cout << "downloadObject: objectIndex is out of range" << std::endl;
    return false;
  }

  long long id = this->m_datasets[objectIndex]->m_id;
  int part = this->m_datasets[objectIndex]->m_part;
  std::string md5 = this->m_datasets[objectIndex]->m_md5;

  std::stringstream url;
  if (m_host.find("http") != 0) {
    url << "http://";
  }
  url << m_host << "/PWService/WebGL?"
      << "sid=" << m_sessionId
      << "&vid=" << m_viewId
      << "&q=" << "mesh"
      << "&id=" << id
      << "&part=" << part+1
      << "&hash=" << md5;

  curl_easy_reset(this->m_curl);
  curl_easy_setopt(m_curl, CURLOPT_URL, url.str().c_str());
  curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, download_dataset);
  curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, m_datasets[objectIndex].get());

  CURLcode result = curl_easy_perform(m_curl);

  if (result == CURLE_OK) {
    m_datasets[objectIndex]->initFromBuffer();
  }

  return true;
}

bool vesPVWebClient::parseSceneMetaData(const std::stringstream& resp)
{
  //strPrint("poll scene response", resp.str());
  jsonSharedPtr respJson = makeShared(cJSON_Parse(resp.str().c_str()));

  if (respJson) {

    // parse renderer
    cJSON* renderersJson = cJSON_GetObjectItem(respJson.get(), "Renderers");

    m_lookAt.clear();
    m_backgroundColor.clear();

    if (renderersJson) {

      cJSON* rendererJson = cJSON_GetArrayItem(renderersJson, 0);

      cJSON* lookAtJson = cJSON_GetObjectItem(rendererJson, "LookAt");
      if (lookAtJson) {
        for (int i = 0; i < 10; ++i) {
          m_lookAt.push_back(cJSON_GetArrayItem(lookAtJson, i)->valuedouble);
        }
      }

      cJSON* backgroundJson = cJSON_GetObjectItem(rendererJson, "Background1");
      if (backgroundJson) {
        for (int i = 0; i < 3; ++i) {
          m_backgroundColor.push_back(cJSON_GetArrayItem(backgroundJson, i)->valuedouble);
        }
      }

      backgroundJson = cJSON_GetObjectItem(rendererJson, "Background2");
      if (backgroundJson) {
        for (int i = 0; i < 3; ++i) {
          m_backgroundColor.push_back(cJSON_GetArrayItem(backgroundJson, i)->valuedouble);
        }
      }
    }


    // parse objects
    cJSON* objectsJson = cJSON_GetObjectItem(respJson.get(), "Objects");

    int numberOfObjects = cJSON_GetArraySize(objectsJson);

    //printf("parsing %d objects\n", numberOfObjects);

    for (int i = 0; i < numberOfObjects; ++i) {
      cJSON* objectJson = cJSON_GetArrayItem(objectsJson, i);
      int parts = cJSON_GetObjectItem(objectJson, "parts")->valueint;
      for (int part = 0; part < parts; ++part) {
        vesPVWebDataSet::Ptr dataset = vesPVWebDataSet::Ptr(new vesPVWebDataSet());
        dataset->m_id = static_cast<long long>(cJSON_GetObjectItem(objectJson, "id")->valuedouble);
        dataset->m_layer = cJSON_GetObjectItem(objectJson, "layer")->valueint;
        dataset->m_md5 = cJSON_GetObjectItem(objectJson, "md5")->valuestring;
        dataset->m_transparency = cJSON_GetObjectItem(objectJson, "transparency")->valueint;
        dataset->m_part = part;
        this->m_datasets.push_back(dataset);
      }
    }
  }
  else {
    this->defaultResponseErrorMessage();
    //printf("response json parse error\n");
    return false;
  }

  return true;
}

bool vesPVWebClient::pollSceneMetaData()
{
  this->m_datasets.clear();

  if (m_sessionId.empty()) {
    //std::cout << "pollSceneMetaData: session id is not initialized" << std::endl;
    return false;
  }

  if (m_viewId.empty()) {
    //std::cout << "pollSceneMetaData: view id is not initialized" << std::endl;
    return false;
  }


  std::stringstream url;
  if (m_host.find("http") != 0) {
    url << "http://";
  }
  url << m_host << "/PWService/WebGL?"
      << "sid=" << m_sessionId
      << "&vid=" << m_viewId
      << "&q=" << "meta";

  //printf("get url: %s\n", url.str().c_str());

  curl_easy_reset(this->m_curl);

  std::stringstream resp;

  curl_easy_setopt(m_curl, CURLOPT_URL, url.str().c_str());
  curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &resp);

  CURLcode result = curl_easy_perform(m_curl);
  if (result == CURLE_OK) {
    return this->parseSceneMetaData(resp);
  }
  else {
    this->defaultCurlErrorMessage();
    //printf("curl_easy_perform() returned an error code: %d\n", result);
    return false;
  }
}


jsonSharedPtr vesPVWebClient::rpc(const std::string& method, cJSON* params, int connectTimeout)
{

  params = params ? params : cJSON_CreateArray();
  jsonSharedPtr jsonData = makeShared(make_json(this->m_id, method, params));
  std::string jsonStr = jsonToStr(jsonData);
  jsonSharedPtr resultJson;

  //strPrint("sending", jsonStr);

  curl_easy_reset(this->m_curl);

  std::stringstream resp;

  std::string url = m_host + "/PWService/json";
  if (url.find("http") != 0) {
    url = "http://" + url;
  }

  //printf("url: %s\n", url.c_str());

  curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
  curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &resp);
  curl_easy_setopt(m_curl, CURLOPT_CONNECTTIMEOUT, connectTimeout);
  curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, connectTimeout);

  CURLcode result = curl_easy_perform(m_curl);
  if (result == CURLE_OK)
    {

    std::string respStr = resp.str();

    //strPrint("response", respStr);

    jsonSharedPtr respJson = makeShared(cJSON_Parse(respStr.c_str()));

    if (respJson)
      {
      cJSON* errorJson = cJSON_GetObjectItem(respJson.get(), "error");
      if (errorJson && errorJson->type != cJSON_NULL)
        {
        //printf("json response contains an error\n");

        cJSON* msgJson = cJSON_GetObjectItem(errorJson, "msg");
        if (msgJson && msgJson->type == cJSON_String)
          {
          this->setError("Response From Server", msgJson->valuestring);
          }
        else
          {
          this->defaultResponseErrorMessage();
          }
        }
      else
        {
        resultJson = makeShared(cJSON_DetachItemFromObject(respJson.get(), "result"));
        }
      }
    else
      {
      //printf("json response parse error\n");
      this->defaultResponseErrorMessage();
      }
    }
  else
    {
    this->defaultCurlErrorMessage();
    //printf("curl_easy_perform() returned an error code: %d\n", result);
    }

  return resultJson;
}

bool vesPVWebClient::createVisualization()
{

  std::string sessionName = "KiwiViewerDemo";
  std::string sessionComment = "no comment";
  std::string sessionType = "default";

  cJSON* params = cJSON_CreateArray();
  cJSON_AddItemToArray(params, cJSON_CreateString(sessionName.c_str()));
  cJSON_AddItemToArray(params, cJSON_CreateString(sessionComment.c_str()));
  cJSON_AddItemToArray(params, cJSON_CreateString(sessionType.c_str()));

  int timeout = 15;
  jsonSharedPtr result = this->rpc("VisualizationsManager.createVisualization", params, timeout);


  if (result && result->type == cJSON_String)
    {
    this->m_sessionId = result->valuestring;
    return true;
    }
  else
    {
    //std::cout << "createVisualization failed" << std::endl;
    return false;
    }
}

bool vesPVWebClient::endVisualization()
{

  if (m_sessionId.empty()) {
    //std::cout << "endVisualization: session id is not initialized" << std::endl;
    return false;
  }

  cJSON* params = cJSON_CreateArray();
  cJSON_AddItemToArray(params, cJSON_CreateString(m_sessionId.c_str()));

  jsonSharedPtr result = this->rpc("VisualizationsManager.stopVisualization", params);

  if (result && result->type == cJSON_String)
    {
    //std::cout << result->valuestring << std::endl;
    m_sessionId.clear();
    return true;
    }
  else
    {
    //std::cout << "stopVisualization failed" << std::endl;
    return false;
    }
}


jsonSharedPtr vesPVWebClient::executeCommand(const std::string& command, cJSON* params, int connectTimeout)
{

  params = params ? params : cJSON_CreateArray();
  jsonSharedPtr myjson = makeShared(make_json(m_id, "execute_command", make_json(m_id, command, params)));


  cJSON* invokeParams = cJSON_CreateArray();
  cJSON_AddItemToArray(invokeParams, cJSON_CreateString(m_sessionId.c_str()));
  cJSON_AddItemToArray(invokeParams, cJSON_CreateString(jsonToStr(myjson.get()).c_str()));

  return this->rpc("VisualizationsManager.invoke", invokeParams, connectTimeout);
}

bool vesPVWebClient::configure(int enableRendering, int imageDelivery, int enableWebGL)
{

  if (m_sessionId.empty()) {
    //std::cout << "configure: session id is not initialized" << std::endl;
    return false;
  }

  int maxSize = 10000000;

  cJSON* params = cJSON_CreateArray();
  cJSON_AddItemToArray(params, cJSON_CreateString(m_sessionId.c_str()));
  cJSON_AddItemToArray(params, cJSON_CreateString("Configure"));

  std::stringstream configureParams;
  configureParams << enableRendering << " "
                  << imageDelivery << " "
                  << enableWebGL << " "
                  << maxSize;


  cJSON_AddItemToArray(params, cJSON_CreateString(configureParams.str().c_str()));
  jsonSharedPtr result = this->rpc("VisualizationsManager.forwardWithoutReply", params);
  return result;
}

void vesPVWebClient::configureOn()
{
  this->configure(1, 1, 1);
}

void vesPVWebClient::configureOff()
{
  this->configure(0, 1, -1);
}

std::string vesPVWebClient::errorTitle() const
{
  return this->mErrorTitle;
}

std::string vesPVWebClient::errorMessage() const
{
  return this->mErrorMessage;
}

void vesPVWebClient::resetErrorMessages()
{
  this->mErrorTitle = std::string();
  this->mErrorMessage = std::string();
}

void vesPVWebClient::setError(const std::string& errorTitle, const std::string& errorMessage)
{
  // preserve the existing error message if one already exists
  if (this->mErrorMessage.empty()) {
    this->mErrorTitle = errorTitle;
    this->mErrorMessage = errorMessage;
  }
}

void vesPVWebClient::defaultResponseErrorMessage()
{
  this->setError("Response Error", "There was an error in the server response.  The server might be down, or there might be a network issue.");
}

void vesPVWebClient::defaultCurlErrorMessage()
{
  this->setError("Network error", "Failed to contact " + m_host);
}

bool vesPVWebClient::createView()
{

  if (!this->m_viewId.empty()) {
    //std::cout << "createView: viewId is already initialized" << std::endl;
    return false;
  }

  int timeout = 15;
  jsonSharedPtr result = this->executeCommand("CreateIfNeededRenderView", NULL, timeout);

  if (result && result->type == cJSON_String) {

    result = makeShared(cJSON_Parse(result->valuestring));

    if (result) {
      cJSON* subResult = cJSON_GetObjectItem(result.get(), "result");
      subResult = cJSON_GetObjectItem(subResult, "result");
      subResult = cJSON_GetObjectItem(subResult, "__selfid__");
      this->m_viewId = subResult->valuestring;
    }
    else {
      this->defaultResponseErrorMessage();
    }
  }
  else {
    //std::cout << "CreateIfNeededRenderView failed" << std::endl;
  }

  return !this->m_viewId.empty();
}

void vesPVWebClient::setHost(const std::string& host)
{
  this->m_host = host;
}

void vesPVWebClient::setSessionId(const std::string& sessionId)
{
  this->m_sessionId = sessionId;
}

const std::vector<vesSharedPtr<vesPVWebDataSet> >& vesPVWebClient::datasets() const
{
  return this->m_datasets;
}

const std::vector<double>& vesPVWebClient::lookAt() const
{
  return m_lookAt;
}

const std::vector<double>& vesPVWebClient::backgroundColor() const
{
  return m_backgroundColor;
}
