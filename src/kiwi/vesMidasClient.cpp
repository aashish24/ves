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

#include "vesMidasClient.h"
#include "vesKiwiCurlDownloader.h"

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




vesMidasClient::vesMidasClient()
{
  this->m_curl = curl_easy_init();
  if (!this->m_curl) {
    std::cout << "error initializing CURL object" << std::endl;
    this->setError("cURL Error", "There was an error initializing cURL.");
  }
}

vesMidasClient::~vesMidasClient()
{
  curl_easy_cleanup(this->m_curl);
}

/** Join arguments into a single string, escaping each parameter name and value.
  *
  * Escaping should only be done for data encoded
  * as application/x-www-form-urlencoded and
  * not multipart/formdata. This is safe for
  * data sent via CURLOPT_COPYPOSTFIELDS but
  * not CURLOPT_POSTFIELDS when CURLOPT_CUSTOMREQUEST
  * sets the content type to multipart/formdata.
  */
std::string vesMidasClient::argList(const RequestArgs& args)
{
  std::stringstream argString;
  char* escParam;
  char* escValue;
  for (RequestArgs::const_iterator iter = args.begin(); iter != args.end(); ++iter) {
    escParam = curl_easy_escape(this->m_curl, iter->first.c_str(), iter->first.size());
    escValue = curl_easy_escape(this->m_curl, iter->second.c_str(), iter->second.size());
    argString << "&" << escParam << "=" << escValue;
    curl_free(escParam);
    curl_free(escValue);
  }
  return argString.str();
}

std::string vesMidasClient::methodUrl(const std::string& method)
{
  return this->m_host + "/api/json?method=" + method;
}


jsonSharedPtr vesMidasClient::request(const std::string& method, const RequestArgs& args, bool tryTokenRenewal)
{
  if (!this->token().size()) {
    this->setError("Request Error", "Invalid request token.");
  }

  jsonSharedPtr resultJson;

  curl_easy_reset(this->m_curl);

  std::string requestUrl = this->methodUrl(method);
  std::string postArgs = this->argList(args);


  //strPrint("post " + method, "url: " + requestUrl + "\npost args: " + postArgs);

  curl_easy_reset(this->m_curl);

  std::stringstream responseData;
  curl_easy_setopt(m_curl, CURLOPT_URL, requestUrl.c_str());
  curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data);
  curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &responseData);
  curl_easy_setopt(m_curl, CURLOPT_COPYPOSTFIELDS, postArgs.c_str());
  curl_easy_setopt(m_curl, CURLOPT_CONNECTTIMEOUT, 30);

  CURLcode result = curl_easy_perform(m_curl);
  if (result == CURLE_OK) {

    //strPrint(method + " response:", responseData.str());
    jsonSharedPtr respJson = makeShared(cJSON_Parse(responseData.str().c_str()));

    if (respJson) {
      std::string statusStr = cJSON_GetObjectItem(respJson.get(), "stat")->valuestring;
      if (statusStr == "ok") {
          resultJson = makeShared(cJSON_DetachItemFromObject(respJson.get(), "data"));
        if (!resultJson) {
          //printf("response json did not have 'data' field\n");
        }
      }
      else {

        // if it's a token error, maybe try renewing the token
        if (cJSON_GetObjectItem(respJson.get(), "message")) {
          std::string messageStr = cJSON_GetObjectItem(respJson.get(), "message")->valuestring;
          if (messageStr == "Invalid token" && tryTokenRenewal) {
            //printf("renewing token\n");
            this->renewToken();
            RequestArgs newArgs = args;
            newArgs["token"] = this->token();
            return this->request(method, newArgs, false);
          }
        }

      this->defaultResponseErrorMessage();
      }
    }
    else {
      this->defaultResponseErrorMessage();
      //printf("response json parse error\n");
    }
  }
  else {
    this->defaultCurlErrorMessage();
    //printf("curl_easy_perform() returned an error code: %d\n", result);
  }

  return resultJson;
}

std::string vesMidasClient::errorTitle() const
{
  return this->mErrorTitle;
}

std::string vesMidasClient::errorMessage() const
{
  return this->mErrorMessage;
}

void vesMidasClient::resetErrorMessages()
{
  this->mErrorTitle = std::string();
  this->mErrorMessage = std::string();
}

void vesMidasClient::setError(const std::string& errorTitle, const std::string& errorMessage)
{
  // preserve the existing error message if one already exists
  if (this->mErrorMessage.empty()) {
    this->mErrorTitle = errorTitle;
    this->mErrorMessage = errorMessage;
  }
}

void vesMidasClient::defaultResponseErrorMessage()
{
  this->setError("Response Error", "There was an error in the server response.  The server might be down, or there might be a network issue.");
}

void vesMidasClient::defaultCurlErrorMessage()
{
  this->setError("Network error", "Failed to contact " + m_host);
}

bool vesMidasClient::login(const std::string& username, const std::string& password)
{
  this->m_apikey = std::string();
  this->m_email = username;

  RequestArgs args;
  args["password"] = password;
  args["email"] = username;
  std::string method = "midas.user.apikey.default";
  jsonSharedPtr response = this->request(method, args);
  if (response) {
    cJSON* apikeyJson = cJSON_GetObjectItem(response.get(), "apikey");
    if (apikeyJson) {
      this->m_apikey = apikeyJson->valuestring;
      return this->renewToken();
    }
  }
  return false;
}

bool vesMidasClient::renewToken()
{
  RequestArgs args;
  args["appname"] = "Default";
  args["email"] = this->m_email;
  args["apikey"] = this->m_apikey;
  std::string method = "midas.login";
  jsonSharedPtr response = this->request(method, args);
  if (response) {
    cJSON* tokenJson = cJSON_GetObjectItem(response.get(), "token");
    if (tokenJson) {
      this->m_token = tokenJson->valuestring;
      return true;
    }
  }
  return false;
}

namespace {

bool parseFoldersJson(cJSON* foldersJson, std::vector<std::string>& folderNames, std::vector<std::string>& folderIds)
{
  if (!foldersJson) {
    return false;
  }

  const int numberOfFolders = cJSON_GetArraySize(foldersJson);
  for (int i = 0; i < numberOfFolders; ++i) {
    cJSON* folderJson = cJSON_GetArrayItem(foldersJson, i);
    std::string folderName = cJSON_GetObjectItem(folderJson, "name")->valuestring;
    std::string folderId = cJSON_GetObjectItem(folderJson, "folder_id")->valuestring;
    folderNames.push_back(folderName);
    folderIds.push_back(folderId);
  }

  return true;
}

}

bool vesMidasClient::listUserFolders()
{
  m_folderNames.clear();
  m_folderIds.clear();

  RequestArgs args;
  args["token"] = this->m_token;
  std::string method = "midas.user.folders";
  jsonSharedPtr response = this->request(method, args);

  if (response) {
    cJSON* foldersJson = response.get();
    return parseFoldersJson(foldersJson, m_folderNames, m_folderIds);
  }
  return false;
}

bool vesMidasClient::listCommunityChildren(const std::string& communityId)
{
  m_folderNames.clear();
  m_folderIds.clear();

  RequestArgs args;
  if (this->m_token.size()) args["token"] = this->m_token;
  args["id"] = communityId;
  std::string method = "midas.community.children";
  jsonSharedPtr response = this->request(method, args);

  if (response) {
    cJSON* foldersJson = cJSON_GetObjectItem(response.get(), "folders");
    return parseFoldersJson(foldersJson, m_folderNames, m_folderIds);
  }
  return false;
}

bool vesMidasClient::listFolderChildren(const std::string& inputFolderId)
{
  m_folderNames.clear();
  m_folderIds.clear();
  m_itemNames.clear();
  m_itemIds.clear();
  m_itemBytes.clear();


  RequestArgs args;
  if (this->m_token.size()) args["token"] = this->m_token;
  args["id"] = inputFolderId;
  std::string method = "midas.folder.children";
  jsonSharedPtr response = this->request(method, args);

  if (response) {
    cJSON* foldersJson = cJSON_GetObjectItem(response.get(), "folders");
    cJSON* itemsJson = cJSON_GetObjectItem(response.get(), "items");

    if (!parseFoldersJson(foldersJson, m_folderNames, m_folderIds)) {
      return false;
    }

    if (itemsJson) {
      const int numberOfItems = cJSON_GetArraySize(itemsJson);
      for (int i = 0; i < numberOfItems; ++i) {
        cJSON* itemJson = cJSON_GetArrayItem(itemsJson, i);
        std::string itemName = cJSON_GetObjectItem(itemJson, "name")->valuestring;
        std::string itemId = cJSON_GetObjectItem(itemJson, "item_id")->valuestring;
        std::string itemBytes = cJSON_GetObjectItem(itemJson, "sizebytes")->valuestring;
        m_itemNames.push_back(itemName);
        m_itemIds.push_back(itemId);
        m_itemBytes.push_back(atol(itemBytes.c_str()));
      }
    }
    return true;
  }
  return false;
}

bool vesMidasClient::listCommunities()
{
  m_folderNames.clear();
  m_folderIds.clear();

  RequestArgs args;
  if (this->m_token.size()) args["token"] = this->m_token;
  std::string method = "midas.community.list";
  jsonSharedPtr response = this->request(method, args);

  if (response) {

    const int numberOfCommunities = cJSON_GetArraySize(response.get());
    for (int i = 0; i < numberOfCommunities; ++i) {
      cJSON* communityJson = cJSON_GetArrayItem(response.get(), i);
      std::string communityName = cJSON_GetObjectItem(communityJson, "name")->valuestring;
      std::string communityId = cJSON_GetObjectItem(communityJson, "community_id")->valuestring;
      m_folderNames.push_back(communityName);
      m_folderIds.push_back(communityId);
    }
    return true;
  }
  return false;
}

std::string vesMidasClient::itemDownloadUrl(const std::string& itemId)
{
  RequestArgs args;
  if (this->m_token.size()) args["token"] = this->m_token;
  args["id"] = itemId;
  std::string method = "midas.item.download";
  std::string requestUrl = this->methodUrl(method) + this->argList(args);

  return requestUrl;
}


/*

midas.folder.children item desc:

{
"item_id":"23821"
"name":"teapot dataset",
"description":"teapot dataset",
"type":"0",
"sizebytes":"14388",
"date_creation":"2012-09-29 22:33:34",
"date_update":"2012-09-29 23:06:52",
"thumbnail_id":null,
"view":"4",
"download":"24",
"privacy_status":"2",
"uuid":"5067af7e1a19a6f27d0643708bff4e342cb626bb86abe",
"extraFields":[]
}

midas.item.get  token=...&id=...

{
"item_id":"23821",
"name":"teapot dataset",
"description":"teapot dataset",
"type":"0",
"sizebytes":"14388",
"date_creation":"2012-09-29 22:33:34",
"date_update":"2012-09-29 23:06:52",
"thumbnail_id":null,
"view":"5",
"download":"24",
"privacy_status":"2",
"uuid":"5067af7e1a19a6f27d0643708bff4e342cb626bb86abe",
"folder_id":"1272",
"revisions":[
  {"itemrevision_id":"23993",
   "item_id":"23821",
   "revision":"1",
   "date":"2012-09-29 22:33:34",
   "changes":"Initial revision",
   "user_id":"12",
   "license_id":"1",
   "uuid":"5067af7e21dcb66465ce5b683d79d55b8cafc98e7714c",
   "bitstreams":[
     {"bitstream_id":"139931",
     "itemrevision_id":"23993",
     "assetstore_id":"1",
     "name":"teapot.vtp",
     "mimetype":"application\/octet-stream",
     "sizebytes":"14388",
     "checksum":"4c8a51ca6cb8b85ff532d99238ff27ee",
     "path":"4c\/8a\/4c8a51ca6cb8b85ff532d99238ff27ee",
     "date":"2012-09-29 22:33:34"}]
   }],
"extraFields":[]
}

*/

void vesMidasClient::reset()
{
  this->m_token = "";
  this->m_apikey = "";
  this->m_host = "";
  this->m_email = "";
  this->m_folderNames.clear();
  this->m_folderIds.clear();
  this->m_itemNames.clear();
  this->m_itemIds.clear();
  this->m_itemBytes.clear();
}

const std::vector<std::string>& vesMidasClient::folderNames()
{
  return m_folderNames;
}

const std::vector<std::string>& vesMidasClient::folderIds()
{
  return m_folderIds;
}

const std::vector<std::string>& vesMidasClient::itemNames()
{
  return m_itemNames;
}

const std::vector<std::string>& vesMidasClient::itemIds()
{
  return m_itemIds;
}

const std::vector<size_t>& vesMidasClient::itemBytes()
{
  return m_itemBytes;
}

void vesMidasClient::setHost(const std::string& host)
{
  this->m_host = host;
}

void vesMidasClient::setToken(const std::string& token)
{
  this->m_token = token;
}

const std::string& vesMidasClient::token() const
{
  return this->m_token;
}
