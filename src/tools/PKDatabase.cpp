//
// ProjectKetchup:
// ---------------
//
// Copyright (c) 2008 Damien Chavarria
//

#include "PKDatabase.h"
#include "PKObjectLoader.h"
#include "PKStr.h"
#include "sqlite3.h"

//
// PKDatabase
//

PKDatabase::PKDatabase()
{
    this->sqlite3_handle = NULL;
}

PKDatabase::~PKDatabase()
{
    // Safety catch
    this->closeDB();
}

//
// Open / Create / Close
//

PKDBErr PKDatabase::openDB(std::wstring filename)
{
    sqlite3 *db;
    int      rc;
    /*
    rc = sqlite3_open_v2(PKStr::wStringToUTF8string(filename).c_str(), 
                         &db, SQLITE_OPEN_READWRITE, NULL);
	 */
    rc = sqlite3_open(PKStr::wStringToUTF8string(filename).c_str(), &db);

    if(rc)
    {
        return PKDB_NOT_FOUND;
    }

    this->sqlite3_handle = (void *) db;

    return PKDB_OK;
}

PKDBErr PKDatabase::createDB(std::wstring filename)
{
    sqlite3 *db;
    int      rc;
    
	/*
    rc = sqlite3_open_v2(PKStr::wStringToUTF8string(filename).c_str(), 
                         &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, NULL);
	 */
    rc = sqlite3_open(PKStr::wStringToUTF8string(filename).c_str(), &db);

    if(rc)
    {
        return PKDB_CANT_CREATE;
    }

    this->sqlite3_handle = (void *) db;

    return PKDB_OK;
}

PKDBErr PKDatabase::closeDB()
{
    if(this->sqlite3_handle == NULL)
    {
        return PKDB_OK;
    }

    sqlite3_close((sqlite3 *) this->sqlite3_handle);
    this->sqlite3_handle = NULL;

    return PKDB_OK;
}

//
// Generic SQL run
//

PKDBErr PKDatabase::runSQL(std::string SQL)
{
    if(this->sqlite3_handle == NULL)
    {
        return PKDB_WRONG_STATE;
    }

    int rc = sqlite3_exec((sqlite3*) this->sqlite3_handle, SQL.c_str(), NULL, NULL, NULL);

    if(rc != SQLITE_OK)
    {
        return PKDB_SQL_ERROR;
    }

    return PKDB_OK;
}

//
// Read
//

PKDBErr PKDatabase::readFirstObjectFromTable(std::string tableName, 
                                             PKObject   *intoObject)
{
    PKObjectLoader loader;

    if(this->sqlite3_handle == NULL)
    {
        return PKDB_WRONG_STATE;
    }

    if(intoObject == NULL)
    {
        return PKDB_INVALID_PARAM;
    }

    // Prepare the SQL statement

    std::string SQL = "SELECT * FROM " + tableName + ";";

    int           rc;
    sqlite3_stmt *stmt = NULL;

    rc = sqlite3_prepare_v2((sqlite3*) this->sqlite3_handle,
                            SQL.c_str(), SQL.size(),
                            &stmt, NULL);

    if(rc != SQLITE_OK)
    {
        return PKDB_WRONG_STATE;
    }
    
    // Now run it only once

    rc = sqlite3_step(stmt);

    if(rc == SQLITE_ROW)
    {
        int column_count = sqlite3_column_count(stmt);

        for(int i=0; i<column_count; i++)
        {
            const char *name = sqlite3_column_name(stmt, i);
            std::string str_name = name;

			// Check column type
			int type = sqlite3_column_type(stmt, i);

			if(type == SQLITE_BLOB)
			{
				// Load the binary data & set it manually

				const void *data = sqlite3_column_blob(stmt, i);
				int         size = sqlite3_column_bytes(stmt, i);

				if(size > 0)
				{
					PKVariantBinary bin(size, (void *) data);
					intoObject->set(str_name, &bin);
				}
			}
			else
			{
				const unsigned char *value = sqlite3_column_text(stmt, i);

				if(value == NULL)
				{
					loader.setObjectProperty(intoObject, str_name, "");
				}
				else
				{
					std::string str_value = (char *) value;

					// Now set the property

					loader.setObjectProperty(intoObject, 
											 str_name, 
											 str_value);
				}
			}
        }

        sqlite3_finalize(stmt);
        stmt = NULL;

        return PKDB_OK;
    }

    sqlite3_finalize(stmt);
    stmt = NULL;

    return PKDB_NOT_FOUND;
}

PKDBErr PKDatabase::getObjectsFromTableWithCreateFunction(std::string             tableName,
														  std::vector<PKObject*> *outObjects,
														  PKObjectCreateFunction  createFunc)
{
    PKObjectLoader loader;
	
    if(this->sqlite3_handle == NULL)
    {
        return PKDB_WRONG_STATE;
    }
	
    if(outObjects == NULL || createFunc == NULL)
    {
        return PKDB_INVALID_PARAM;
    }
	
    // Prepare the SQL statement
	
    std::string SQL = "SELECT * FROM " + tableName + ";";
	
    int           rc;
    sqlite3_stmt *stmt = NULL;
	
    rc = sqlite3_prepare_v2((sqlite3*) this->sqlite3_handle,
                            SQL.c_str(), SQL.size(),
                            &stmt, NULL);
	
    if(rc != SQLITE_OK)
    {
        return PKDB_WRONG_STATE;
    }
    
    // Now run it
	
    do
    {
        rc = sqlite3_step(stmt);
		
        if(rc == SQLITE_ROW)
        {
            // Create a new PKObject
			
            PKObject *obj = createFunc();
			
			if(obj == NULL)
			{
				break;
			}
			
            int column_count = sqlite3_column_count(stmt);
			
            for(int i=0; i<column_count; i++)
            {
                const char *name = sqlite3_column_name(stmt, i);
                std::string str_name = name;

				// Check column type
				int type = sqlite3_column_type(stmt, i);

				if(type == SQLITE_BLOB)
				{
					// Load the binary data & set it manually

					const void *data = sqlite3_column_blob(stmt, i);
					int         size = sqlite3_column_bytes(stmt, i);

					if(size > 0)
					{
						PKVariantBinary bin(size, (void *) data);
						obj->set(str_name, &bin);
					}
				}
				else
				{
					const unsigned char *value = sqlite3_column_text(stmt, i);

					if(value == NULL)
					{
						loader.setObjectProperty(obj, str_name, "");
					}
					else
					{
						std::string str_value = (char *) value;
						
						// Now set the property
						loader.setObjectProperty(obj, 
												 str_name, 
												 str_value);
					}
				}
            }
			
            // Add the object
            outObjects->push_back(obj);
        }
        else if(rc == SQLITE_DONE)
        {
            break;
        }
        else
        {
            // This is an error
            sqlite3_finalize(stmt);
            return PKDB_WRONG_STATE;
        }
    }
    while(true);
	
    sqlite3_finalize(stmt);
    stmt = NULL;
	
    return PKDB_OK;
}


PKDBErr PKDatabase::getObjectsFromTable(std::string             tableName,
                                        std::vector<PKObject*> *outObjects)
{
    PKObjectLoader loader;

    if(this->sqlite3_handle == NULL)
    {
        return PKDB_WRONG_STATE;
    }

    if(outObjects == NULL)
    {
        return PKDB_INVALID_PARAM;
    }

    // Prepare the SQL statement

    std::string SQL = "SELECT * FROM " + tableName + ";";

    int           rc;
    sqlite3_stmt *stmt = NULL;

    rc = sqlite3_prepare_v2((sqlite3*) this->sqlite3_handle,
                            SQL.c_str(), SQL.size(),
                            &stmt, NULL);

    if(rc != SQLITE_OK)
    {
        return PKDB_WRONG_STATE;
    }
    
    // Now run it

    do
    {
        rc = sqlite3_step(stmt);

        if(rc == SQLITE_ROW)
        {
            // Create a new PKObject

            PKObject *obj = new PKObject();

            int column_count = sqlite3_column_count(stmt);

            for(int i=0; i<column_count; i++)
            {
                const char *name = sqlite3_column_name(stmt, i);
                std::string str_name = name;
				
				// Check column type
				int type = sqlite3_column_type(stmt, i);

				if(type == SQLITE_BLOB)
				{
					// Load the binary data & set it manually

					const void *data = sqlite3_column_blob(stmt, i);
					int         size = sqlite3_column_bytes(stmt, i);

					// Add the property as binary
					obj->addProperty(str_name, PK_VARIANT_BINARY);

					if(size > 0)
					{
						PKVariantBinary bin(size, (void *) data);
						obj->set(str_name, &bin);
					}
				}
				else
				{
					const unsigned char *value = sqlite3_column_text(stmt, i);

					// Add the property as generic string
					obj->addProperty(str_name, PK_VARIANT_STRING);

					if(value == NULL)
					{
						loader.setObjectProperty(obj, str_name, "");
					}
					else
					{
						std::string str_value = (char *) value;

						// Now set the property
						loader.setObjectProperty(obj, 
												 str_name, 
												 str_value);
					}
				}
            }

            // Add the object
            outObjects->push_back(obj);
        }
        else if(rc == SQLITE_DONE)
        {
            break;
        }
        else
        {
            // This is an error
            sqlite3_finalize(stmt);
            return PKDB_WRONG_STATE;
        }
    }
    while(true);

    sqlite3_finalize(stmt);
    stmt = NULL;

    return PKDB_OK;
}

//
// Create table
//

PKDBErr PKDatabase::createTableFromObjectProperties(std::string tableName,
                                                    PKObject   *structure)
{
    if(this->sqlite3_handle == NULL)
    {
        return PKDB_WRONG_STATE;
    }

    if(structure == NULL)
    {
        return PKDB_INVALID_PARAM;
    }

    // Open the SQL command

    std::string SQL = "CREATE TABLE " + tableName + " (";

    PKPropertyBag *properties = structure->getProperties();

    if(properties == NULL)
    {
        return PKDB_INVALID_PARAM;
    }

    if(properties->getNumberOfProperties() == 0)
    {
        return PKDB_INVALID_PARAM;
    }

    // Print all properties

    for(uint32_t i=0; i<properties->getNumberOfProperties(); i++)
    {
        PKProperty *prop = properties->getPropertyAt(i);

        if(prop)
        {
            std::string name  = prop->getName();
            PKVariant  *value = prop->getValue();

            if((value != NULL) && (value->_type != PK_VARIANT_NULL))
            {
                SQL += name;
                SQL += " ";

                switch(value->_type)
                {
                case PK_VARIANT_INT64:
                case PK_VARIANT_INT32:
                case PK_VARIANT_INT16:
                case PK_VARIANT_INT8:                	
                case PK_VARIANT_UINT64:
                case PK_VARIANT_UINT32:
                case PK_VARIANT_UINT16:
                case PK_VARIANT_UINT8:   
                case PK_VARIANT_VOIDP:                	
                    {
                        SQL += "INT";
                    }
                    break;

                case PK_VARIANT_DOUBLE:
                    {
                        SQL += "REAL";
                    }
                    break;

				case PK_VARIANT_BINARY:
					{
						SQL += "BLOB";
					}
					break;

                case PK_VARIANT_BOOL:
                case PK_VARIANT_STRING:
                case PK_VARIANT_WSTRING:
                case PK_VARIANT_POINT:
                case PK_VARIANT_RECT:
                case PK_VARIANT_COLOR:
                case PK_VARIANT_STRINGLIST:
                case PK_VARIANT_WSTRINGLIST:
                case PK_VARIANT_INT32RANGE:
                case PK_VARIANT_INT64RANGE:
                case PK_VARIANT_DOUBLERANGE:
                    {
                        SQL += "TEXT";
                    }
                    break;

                default:
                    {
                        SQL += "TEXT";
                    }
                    break;
                }

                if(i != (properties->getNumberOfProperties() -1))
                {
                    SQL += ", ";
                }
            }
        }
    }

    // Close it out
    SQL += ");";

    return this->runSQL(SQL);;
}

//
// Write
//

PKDBErr PKDatabase::insertObjectIntoTable(std::string tableName,
                                          PKObject   *object)
{
    if(this->sqlite3_handle == NULL)
    {
        return PKDB_WRONG_STATE;
    }

    if(object == NULL)
    {
        return PKDB_INVALID_PARAM;
    }

    // Open the SQL command

    std::string SQL = "INSERT INTO " + tableName + " (";

    PKPropertyBag *properties = object->getProperties();

    if(properties == NULL)
    {
        return PKDB_INVALID_PARAM;
    }

    if(properties->getNumberOfProperties() == 0)
    {
        return PKDB_INVALID_PARAM;
    }

    // Print all property names

    for(uint32_t i=0; i<properties->getNumberOfProperties(); i++)
    {
        PKProperty *prop = properties->getPropertyAt(i);

        if(prop)
        {
            std::string name  = prop->getName();
            PKVariant  *value = prop->getValue();

            if((value != NULL) && (value->_type != PK_VARIANT_NULL))
            {
                SQL += name;

                if(i != (properties->getNumberOfProperties() -1))
                {
                    SQL += ", ";
                }
            }
        }
    }

    // Close it out
    SQL += ") VALUES (";

    // Now print values

	int blobCount = 0;

    for(uint32_t i=0; i<properties->getNumberOfProperties(); i++)
    {
        PKProperty *prop = properties->getPropertyAt(i);

        if(prop)
        {
            PKVariant  *value = prop->getValue();

            if((value != NULL) && (value->_type != PK_VARIANT_NULL))
            {
				if(value->_type == PK_VARIANT_BINARY)
				{
					SQL += "?";
					blobCount++;
				}
				else
				{
					SQL += "'";
					SQL += PKStr::escapeSingleQuotes(value->toString());
					SQL += "'";
				}

                if(i != (properties->getNumberOfProperties() -1))
                {
                    SQL += ", ";
                }
            }
        }
    }

	// Close it out completely
    SQL += ");";

	if(blobCount > 0)
	{
		char *tail = NULL;
		sqlite3_stmt *stmt = NULL;

		sqlite3_prepare_v2((sqlite3 *) this->sqlite3_handle, 
			  			   SQL.c_str(), SQL.size(), &stmt, (const char **) &tail);

		int count = 0;

		for(uint32_t i=0; i<properties->getNumberOfProperties(); i++)
		{
			PKProperty *prop = properties->getPropertyAt(i);

			if(prop)
			{
				PKVariant *value = prop->getValue();

				if(value->_type == PK_VARIANT_BINARY)
				{
					PKVariantBinary *bin = (PKVariantBinary *) value;

				    sqlite3_bind_blob(stmt, 
						              count+1, 
									  bin->_buffer, 
									  bin->_size, 
								      SQLITE_TRANSIENT);
					count++;
				}
			}
		}

		if(sqlite3_step(stmt) != SQLITE_DONE) 
		{
			sqlite3_finalize(stmt);		
			return PKDB_SQL_ERROR;
		}

		sqlite3_finalize(stmt);		
	
		return PKDB_OK;
	}

    return this->runSQL(SQL);;
}
