//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKObject.h"
#include "PKStr.h"
#include "PKPath.h"
#include "PKFile.h"

#include <string>
#include <vector>

//
// Globals
// -------
//

//
// printUsage()
// ------------
//

void printUsage()
{
    printf("                                                            \n");
    printf("  pki8nlst : PK tool to parse source code and fetch all the \n");
    printf("  --------   strings marked with PK_i18n() for translation. \n");
    printf("                                                            \n");
    printf("  Usage: pki18nlst -i configurationFile -o outputFile       \n");
    printf("  ------                                                    \n");
    printf("                                                            \n");
    printf("  Where: configurationFile is a text file listing the       \n");
    printf("  ------ source directories to include in the crawl,        \n");
    printf("         and outputFile is the strings list file to         \n");
    printf("         produce. A .pot extension will be added to the     \n");
    printf("         output file name if not specified.                 \n");
    printf("                                                            \n");
}

//
// Utils
//



//
// int main()
// ----------
//

int main(int argc, char **argv)
{
    // 
    // Check Usage
    //

    if(argc != 5)
    {
        printUsage();
        return 1;
    }

    // 
    // Check Command Line Parameters
    //

    std::string argv1 = argv[1];
    std::string argv2 = argv[2];
    std::string argv3 = argv[3];
    std::string argv4 = argv[4];

    if(argv1 != "-i")
    {
        printUsage();
        return 1;
    }

    if(argv3 != "-o")
    {
        printUsage();
        return 1;
    }

    //
    // Build the input directory list
    //

    char line[4096];
    std::vector<std::string> inputList;

    FILE *conf = fopen(argv2.c_str(), "r");

    if(conf == NULL)
    {
        printf("\npki18nlst: ERROR Could not open configuration file : %s\n\n", argv2.c_str());
        return 1;
    }

    while(fgets(line, 4096, conf))
    {
        std::string folder = line;

        if(PKStr::isOnlyWhiteSpaceAndTabs(folder))
        {
            continue;
        }

        folder = PKStr::stripBeginingAndEndChar(folder, L' ');
        folder = PKStr::stripBeginingAndEndChar(folder, L'\t');
        folder = PKStr::stripBeginingAndEndChar(folder, L'\n');
        folder = PKStr::stripBeginingAndEndChar(folder, L'\r');

        inputList.push_back(folder);
    }

    fclose(conf);

    //
    // Compute output file name
    //

    std::string outputFilename = argv4;

    if(PKStr::findFirstOf(outputFilename, ".pot") != (outputFilename.size() - 4))
    {
        outputFilename += ".pot";
    }

    //
    // Open the output file
    //

    FILE *output = fopen(outputFilename.c_str(), "wb");

    if(output == NULL)
    {
        printf("\npki18nlst: ERROR Could not open output file : %s\n\n", outputFilename.c_str());
        return 1;
    }

    fprintf(output, "#                            \n");
    fprintf(output, "# Translation template file. \n");
    fprintf(output, "#                            \n");

    char nl;
    char cr;
    nl = 0x0D;
    cr = 0x0A;

    // 
    // Now let's go and process those 
    // folders and look for source files
    //

    if(inputList.size() == 0)
    {
        printf("\npki18nlst: ERROR No directories to process found in : %s\n\n", argv2.c_str());
        return 1;
    }

    unsigned int count=0;

    std::vector<std::string> alreadyAdded;

    for(unsigned int i=0; i < inputList.size(); i++)
    {
        std::wstring folder = PKStr::stringToWString(inputList[i]);

        std::vector<std::wstring> files = PKPath::getFiles(folder, 
                                                           L"h,c,cpp", 
                                                           true);
    
        count += files.size();

        for(unsigned int n=0; n < files.size(); n++)
        {
            std::wstring file = files[n];

            //
            // Open the file and look for i18n strings
            //

            PKFile source;

            if(source.open(file, PKFILE_MODE_READ) == PKFILE_OK)
            {   
                // Read the file

                uint64_t fileSize = source.getFileSize();

                if(fileSize < 256000) // max source code file check
                {
                    char *fileBuffer = new char[(uint32_t)fileSize];

                    if(fileBuffer == NULL)
                    {
                        fclose(output);
                        source.close();
                        printf("\npki18nlst: ERROR Out of Memory\n\n");
                        return 1;
                    }

                    uint32_t amount  = source.read(fileBuffer, (uint32_t)fileSize);

                    if(amount != fileSize)
                    {
                        delete [] fileBuffer;
                        fclose(output);
                        source.close();
                        printf("\npki18nlst: ERROR Unable to read the contents of %s\n\n", PKStr::wStringToString(file));
                        return 1;
                    }

                    std::string fileContents = fileBuffer;

                    // Look for the PK_i18N() strings

                    int offset = 0;

                    do
                    {
                        offset = PKStr::findFirstOf(fileContents, "PK_i18n(L\"");

                        if(offset >= 0)
                        {
                            std::string extracted = "";

                            // Extract the string

                            fileContents = fileContents.substr(offset+10);

                            bool shouldContinue = false;

                            do
                            {
                                int nextOffset = PKStr::findFirstOf(fileContents, "\"");

                                if(nextOffset == std::string::npos)
                                {
                                    // Error ?
                                    break;
                                }

                                if(nextOffset == 0)
                                {
                                    // This the end of the string
                                    fileContents = fileContents.substr(1);
                                    break;
                                }

                                char previous = fileContents[nextOffset-1];

                                if(previous != '\\')
                                {
                                    // We got our string
                                    extracted += fileContents.substr(0, nextOffset);
                                    shouldContinue = false;
                                }
                                else
                                {
                                    extracted += fileContents.substr(0, nextOffset+1);
                                    shouldContinue = true;
                                }

                                fileContents = fileContents.substr(nextOffset+1);
                            }
                            while(shouldContinue);

                            //
                            // Write the string to the file
                            // but only if not empty and also
                            // if not already added
                            // 

                            if(extracted != "")
                            {
                                bool skip = false;

                                for(uint32_t k=0; k<alreadyAdded.size(); k++)
                                {
                                    if(alreadyAdded[k] == extracted)
                                    {
                                        skip = true;
                                        break;
                                    }
                                }

                                if(!skip)
                                {
                                    fprintf(output, "\n");

                                    std::string line1 = "msgid \"";

                                    line1 += extracted;
                                    line1 += "\"";

                                    std::string line2 = "msgstr \"\"";

                                    fwrite(line1.c_str(), line1.size()*sizeof(char), 1, output);
                                    fwrite(&nl, sizeof(char), 1, output);
                                    fwrite(&cr, sizeof(char), 1, output);

                                    fwrite(line2.c_str(), line2.size()*sizeof(char), 1, output);
                                    fwrite(&nl, sizeof(char), 1, output);
                                    fwrite(&cr, sizeof(char), 1, output);
                                
                                    alreadyAdded.push_back(extracted);
                                }
                            }
                        }
                    }
                    while(offset != std::string::npos);

                    // And now look for the PKI18N: XML strings

                    fileContents = fileBuffer;
                    offset = 0;

                    do
                    {
                        offset = PKStr::findFirstOf(fileContents, "\\\"PKI18N:");

                        if(offset >= 0)
                        {
                            std::string extracted = "";

                            // Extract the string

                            fileContents   = fileContents.substr(offset+9);

                            bool shouldContinue = false;

                            do
                            {
                                int nextOffset = PKStr::findFirstOf(fileContents, "\\\"");

                                if(nextOffset == std::string::npos)
                                {
                                    // Error ?
                                    break;
                                }

                                // We got our string
                                extracted += fileContents.substr(0, nextOffset);
                                shouldContinue = false;

                                fileContents = fileContents.substr(nextOffset+1);
                            }
                            while(shouldContinue);

                            //
                            // Write the string to the file
                            //

                            if(extracted != "")
                            {
                                bool addIt = true;

                                for(uint32_t i=0; i < alreadyAdded.size(); i++)
                                {
                                    if(alreadyAdded[i] == extracted)
                                    {
                                        addIt = false;
                                        break;
                                    }
                                }

                                if(addIt)
                                {
                                    alreadyAdded.push_back(extracted);

                                    fprintf(output, "\n");

                                    std::string line1 = "msgid \"";

                                    line1 += extracted;
                                    line1 += "\"";

                                    std::string line2 = "msgstr \"\"";

                                    fwrite(line1.c_str(), line1.size()*sizeof(char), 1, output);
                                    fwrite(&nl, sizeof(char), 1, output);
                                    fwrite(&cr, sizeof(char), 1, output);

                                    fwrite(line2.c_str(), line2.size()*sizeof(char), 1, output);
                                    fwrite(&nl, sizeof(char), 1, output);
                                    fwrite(&cr, sizeof(char), 1, output);
                                }
                            }
                        }
                    }
                    while(offset != std::string::npos);

                    // All done

                    delete [] fileBuffer;
                }
                
                source.close();
            }
        }
    }

    if(count == 0)
    {
        printf("\npki18nlst: ERROR No files to process found in the target folder(s)\n\n");
        return 1;
    }

    // All done

    fclose(output);

    printf("\npki18nlst: Succesfully created %s\n\n", outputFilename.c_str());

    return 0;
}