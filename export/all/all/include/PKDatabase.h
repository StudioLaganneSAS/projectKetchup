//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#ifndef PK_DATABASE_H
#define PK_DATABASE_H

#include "PKObject.h"
#include <string>
#include <vector>

typedef enum
{
    PKDB_OK,
    PKDB_NOT_FOUND,
    PKDB_CANT_CREATE,
    PKDB_INVALID_PARAM,
    PKDB_WRONG_STATE,
    PKDB_SQL_ERROR,

} PKDBErr;

//
// Function prototype for object creation
//

typedef PKObject *(*PKObjectCreateFunction)();

//
// PKDatabase
//

class PKDatabase
{
public:

    PKDatabase();
    ~PKDatabase();

    //
    // Open / Create / Close
    //

    PKDBErr openDB(std::wstring filename);
    PKDBErr createDB(std::wstring filename);
    PKDBErr closeDB();

    //
    // Generic SQL run
    //

    PKDBErr runSQL(std::string SQL);

    //
    // Read
    //

    PKDBErr readFirstObjectFromTable(std::string tableName,   // Name of the table to read from
                                     PKObject   *intoObject); // Pass an existing object, values will be set if they match

	PKDBErr getObjectsFromTableWithCreateFunction(std::string             tableName,   // Name of the table to read from
												  std::vector<PKObject*> *outObjects,  // Pass a pointer to an empty vector, the call will fill it
												  PKObjectCreateFunction  createFunc); // With this creation function
												  
    PKDBErr getObjectsFromTable(std::string             tableName,   // Name of the table to read from
                                std::vector<PKObject*> *outObjects); // Pass a pointer to an empty vector, the call will
                                                                     // create std objects and their properties (all strings)


    //
    // Create table
    //

    PKDBErr createTableFromObjectProperties(std::string tableName,
                                            PKObject   *structure);

    //
    // Write
    //

    PKDBErr insertObjectIntoTable(std::string tableName,
                                  PKObject   *object);

private:

    void *sqlite3_handle;
};

#endif // PK_DATABASE_H
