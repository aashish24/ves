import sys
import os

def writeEncodedStringToFile(fileName, variableName, str):

    outFile = open(fileName, 'w')
    outFile.write('char %s[] = {' % variableName)
    for i in range(0, len(str), 16):
        outFile.write('\n  ')
        for c in str[i:i+16]:
            outFile.write('%d,' % ord(c))
    outFile.write('0\n};\n')

def writeHeader(fileName, variableName):

    outFile = open(fileName, 'w')
    outFile.write(
'''#ifndef __%s_h
#define __%s_h

extern char %s[];

#endif''' % (variableName, variableName, variableName))


def writeFactoryFile(outDir, factoryName, variableNames):

    cppFile = os.path.join(outDir, factoryName + '.cpp')
    headerFile = os.path.join(outDir, factoryName + '.h')

    methodDeclarations = []
    methodDefinitions = []
    includes = []

    for variableName in variableNames:
        methodDeclarations.append('  /// Get the source to the %s.glsl shader program\n  static char* %s();\n' % (variableName, variableName))
        methodDefinitions.append('char* %s::%s() { return ::%s; }' % (factoryName, variableName, variableName))
        includes.append('#include "%s.h"' % variableName)

    outFile = open(headerFile, 'w')
    outFile.write(
'''#ifndef __%s_h

/// %s provides accessors to builtin glsl shader program source code.
/// \ingroup shaders
class %s {
public:
%s
};
#endif
''' % (factoryName, factoryName, factoryName, '\n'.join(methodDeclarations)))

    outFile = open(cppFile, 'w')
    outFile.write(
'''#include "%s.h"

%s

%s
''' % (factoryName, '\n'.join(includes), '\n'.join(methodDefinitions)))


def getContentsOfFile(fileName):
    f = open(fileName, 'r')
    return f.read()


if __name__ == '__main__':
    
    if len(sys.argv) < 3:
        print 'Usage: %s <out dir> <factory name> [shader files ...]' % sys.argv[0]
        sys.exit(1)

    outDir = sys.argv[1]
    factoryName = sys.argv[2]
    inFiles = sys.argv[3:]

    baseNames = []

    for inFile in inFiles:
        baseName = os.path.splitext(os.path.basename(inFile))[0]
        baseNames.append(baseName)
        cppFile = os.path.join(outDir, baseName + '.cpp')
        headerFile = os.path.join(outDir, baseName + '.h')

        writeEncodedStringToFile(cppFile, baseName, getContentsOfFile(inFile))
        writeHeader(headerFile, baseName)

    writeFactoryFile(outDir, factoryName, baseNames)


