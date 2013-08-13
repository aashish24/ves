#!/usr/bin/env python

import sys

def generateExportedTypes(fileTypes, out):

    iconName = 'kiwi_72'

    out.write('<!---------- BEGIN GENERATED CODE ---------->\n')
    out.write('  <key>CFBundleDocumentTypes</key>\n')
    out.write('  <array>\n')

    for extension, description in fileTypes.iteritems():
        out.write(
'''    <dict>
      <key>CFBundleTypeIconFiles</key>
      <array>
        <string>%s</string>
      </array>
      <key>CFBundleTypeName</key>
      <string>%s</string>
      <key>CFBundleTypeRole</key>
      <string>Viewer</string>
      <key>LSHandlerRank</key>
      <string>Owner</string>
      <key>LSItemContentTypes</key>
      <array>
        <string>com.kitware.%s</string>
      </array>
    </dict>
''' % (iconName, description, extension))


    out.write('  </array>\n')
    out.write('  <key>UTExportedTypeDeclarations</key>\n')
    out.write('  <array>\n')


    for extension, description in fileTypes.iteritems():
        out.write(
'''    <dict>
      <key>UTTypeConformsTo</key>
      <array>
        <string>public.plain-text</string>
        <string>public.text</string>
      </array>
      <key>UTTypeDescription</key>
      <string>%s</string>
      <key>UTTypeIdentifier</key>
      <string>com.kitware.%s</string>
      <key>UTTypeReferenceURL</key>
      <string>http://www.vtk.org/</string>
      <key>UTTypeSize64IconFile</key>
      <string>%s</string>
      <key>UTTypeTagSpecification</key>
      <dict>
        <key></key>
        <string></string>
        <key>public.filename-extension</key>
        <string>%s</string>
        <key>public.mime-type</key>
        <string>%s</string>
      </dict>
    </dict>
''' % (description, extension, iconName, extension, extension))

    out.write('  </array>\n')
    out.write('<!---------- END GENERATED CODE ---------->\n')



def main():

    fileTypes = {
        'vtk':'VTK File',
        'vtp':'VTK Poly Data File',
        'vtu':'VTK Unstructured Grid File',
        'vti':'VTK Image File',
        'vtm':'VTK Multi Block File',
        'kiwi':'Kiwi Scene File',
        'mha':'MetaImage File',
        'byu':'BYU File',
        'stl':'STL File',
        'g':'Geometry File',
        'tiff':'Tiff Image File',
        'nrrd':'Nrrd Image File',
        'pcd':'Point Cloud Library Data File',
        'pdb':'Protein Data Bank File',
        'obj':'OBJ File',
        'ply':'PLY File',
        'jpg':'JPEG File',
        'png':'PNG File',
        'zip':'ZIP File',
        'gz':'GZip File',
        'bz2':'BZip2 File',
        'tgz':'Tarball File',
        }

    #print ', '.join(sorted(fileTypes.keys()))
    generateExportedTypes(fileTypes, sys.stdout)


if __name__ == '__main__':
    main()
