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
    printf("  pki8nstrip : PK tool to parse strip duplicates from a     \n");
    printf("  ----------   translation template file (.pot)             \n");
    printf("                                                            \n");
    printf("  Usage: pki18strip -i file                                 \n");
    printf("  ------                                                    \n");
    printf("                                                            \n");
    printf("  Where: file is a .pot file.                               \n");
    printf("                                                            \n");
}

//
// int main()
// ----------
//

int main(int argc, char **argv)
{
    // 
    // Check Usage
    //

    if(argc != 3)
    {
        printUsage();
        return 1;
    }

    // 
    // Check Command Line Parameters
    //

    std::string argv1 = argv[1];
    std::string argv2 = argv[2];

    if(argv1 != "-i")
    {
        printUsage();
        return 1;
    }

    //
    // Build the input directory list
    //

    char line[4096];
    char msgid[4096];
    std::vector<std::string> inputList;

    FILE *conf = fopen(argv2.c_str(), "r");

    if(conf == NULL)
    {
        printf("\npki18strip: ERROR Could not open input file : %s\n\n", argv2.c_str());
        return 1;
    }

    while(fgets(line, 4096, conf))
    {
        msgid[0] = 0;
        
        if(sscanf(line, "msgid \"%s", &msgid) > 0)
        {
            std::string id = &(line[7]);
            int quote_pos  = id.find_last_of("\"");
            id = id.substr(0, quote_pos);

            // Check if we added it before
            bool already_added = false;
            for(uint32_t i=0; i<inputList.size(); i++)
            {
                if(id == inputList[i])
                {
                    already_added = true;
                    break;
                }
            }

            if(!already_added)
            {
                inputList.push_back(id);
            }
        }
    }

    fclose(conf);

    //
    // Open the output file
    //

    FILE *output = fopen(argv2.c_str(), "wb");

    if(output == NULL)
    {
        printf("\npki18nstrip: ERROR Could not open output file : %s\n\n", argv2.c_str());
        return 1;
    }

    fprintf(output, "#                            \n");
    fprintf(output, "# Translation template file. \n");
    fprintf(output, "#                            \n");

    char nl;
    char cr;
    nl = 0x0D;
    cr = 0x0A;

    for(uint32_t n=0; n<inputList.size(); n++)
    {
        fprintf(output, "\n");

        std::string line1 = "msgid \"";

        line1 += inputList[n];
        line1 += "\"";

        std::string line2 = "msgstr \"\"";

        fwrite(line1.c_str(), line1.size()*sizeof(char), 1, output);
        fwrite(&nl, sizeof(char), 1, output);
        fwrite(&cr, sizeof(char), 1, output);

        fwrite(line2.c_str(), line2.size()*sizeof(char), 1, output);
        fwrite(&nl, sizeof(char), 1, output);
        fwrite(&cr, sizeof(char), 1, output);
    }

    fclose(output);

    printf("\npki18strip: Succesfully stripped %s\n\n", argv2.c_str());

    return 0;
}