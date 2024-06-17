#include "libsql.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int starts_with(const char *prefix,const char *string)
// This auxiliary functions test if the string begins with the given prefix.
{
    char c_string,c_prefix;
    while((c_prefix=*prefix++) && (c_string=*string++))
    {
        if(c_prefix !=c_string )
            return 0;
    }

		// If the string runs out before the prefix, we return 0 (false).
		if (!c_string && c_prefix)
			return 0;
		else  
			return 1;
}


int libsql_open_any(const char *url, const char *auth_token, libsql_database_t *out_db, const char **out_err_msg)
{
  if (starts_with("http://",url) || starts_with("libsql://",url))
			return libsql_open_remote(url,auth_token, out_db, out_err_msg);
  else 
			return libsql_open_ext(url, out_db, out_err_msg);
}

libsql_stmt_t insert_stmt;

int insert_person(const char* name,int age,const char **out_err_msg)
{
		fprintf(stderr,"insert_person (%s,%d)\n",name,age);
		int retval=libsql_bind_string(insert_stmt,1, name, out_err_msg);
		if (retval != 0) return retval;
		
		retval=libsql_bind_int(insert_stmt,2, age, out_err_msg);
  	if (retval != 0) return retval;

		retval= libsql_execute_stmt(insert_stmt, out_err_msg);
		if (retval != 0) return retval;

		retval= libsql_reset_stmt(insert_stmt, out_err_msg);
		return retval;
}


int main(int argc, char *argv[])
{
	libsql_database_t db;
	libsql_connection_t conn;
	const char *err = NULL;
	int retval = 0;
	libsql_rows_t rows;
	libsql_row_t row;
	int num_cols;
	int c;
	int rflag=0;
	int aflag=0;

	 while ((c = getopt (argc, argv, "ar")) != -1)
    switch (c)
      {
        case 'a':
					aflag = 1;
					break;

				case 'r':
					rflag = 1;
					break;
      }

  char* url= getenv ("LIBSQL_URL");
  if (!url)
		{
		url=":memory:";
		fprintf(stderr, "Using the default value LIBSQL_URL=%s\n", url);
		}
	else 	
		fprintf(stderr, "Using the environment variable LIBSQL_URL=%s\n", url);
	
	char* auth_token= getenv ("LIBSQL_AUTH_TOKEN");
	if (!auth_token)
				auth_token="";
		retval = libsql_open_remote(url,auth_token, &db, &err);
 
  if (aflag) {
  	fprintf(stderr, "Using libsql_open_any \n");
		retval = libsql_open_any(url,auth_token, &db, &err);
  }
  else if (rflag) {
		fprintf(stderr, "Using libsql_open_remote \n");
		retval = libsql_open_remote(url,auth_token, &db, &err);
  }	
   else {
				fprintf(stderr, "Using libsql_open_ext \n");
				retval = libsql_open_ext(url, &db, &err);
   }
  if (retval != 0) {
		fprintf(stderr, "%s\n", err);
		goto quit;
	}

  retval = libsql_connect(db, &conn, &err);
  if (retval != 0) {
		fprintf(stderr, "%s\n", err);
		goto quit;
	}

 const char* drop_table="DROP TABLE IF EXISTS Persons;";
 fprintf(stderr, "%s\n", drop_table);
 retval = libsql_execute(conn,drop_table, &err);
 if (retval != 0) {
		fprintf(stderr, "%s\n", err);
		goto quit;
	}

		
 const char* create_table="CREATE TABLE Persons(Name TEXT,Age INTEGER);";
  fprintf(stderr, "%s\n", create_table);
  retval = libsql_execute(conn,create_table, &err);
  if (retval != 0) {
		fprintf(stderr, "%s\n", err);
		goto quit;
	}

	const char* insert_sql = "INSERT INTO Persons VALUES (?,?)";
		
	retval=libsql_prepare(conn, insert_sql, &insert_stmt, &err);
	if (retval != 0) {
		fprintf( stderr, "%s\n", err);
		goto quit;
	}

	retval= insert_person("Paul",20,&err);
	if (retval != 0) {
		fprintf( stderr, "%s\n", err);
		goto quit;
	}
	retval= insert_person("Laura",30,&err);
	if (retval != 0) {
		fprintf( stderr, "%s\n", err);
		goto quit;
	}

  retval = libsql_query(conn, "SELECT * FROM Persons;", &rows, &err);
	if (retval != 0) {
		fprintf( stderr, "%s\n", err);
		goto quit;
	}


	num_cols = libsql_column_count(rows);
	//fprintf( core.stdc.stdio.stderr, "num_colds =%d\n", num_cols);
	assert(num_cols==2);

	while ((retval = libsql_next_row(rows, &row, &err)) == 0) {
		if (!err && !row) {
			break;
		}
		for (int col = 0; col < num_cols; col++) {
			if (col==0) {
			  const char* value;
				retval = libsql_get_string(row, col, &value, &err);
				if (retval != 0) {
					fprintf(stderr, "%s\n", err);
				} else {
					printf("%s\t", value);
				}
			} else if (col==1) {
				long long value;
				retval = libsql_get_int(row, col, &value, &err);
				if (retval != 0) {
					fprintf(stderr, "%s\n", err);
				} else {
					printf("%lld\n", value);
				}
			}
			if (retval != 0) {
				fprintf( stderr, "%s\n", err);
				goto quit;
			}
		}
		err = NULL;
	}
	if (retval != 0) {
		fprintf( stderr, "%s\n", err);
	}

 quit:
	if (insert_stmt)	libsql_free_stmt(insert_stmt);
	libsql_free_rows(rows);
	libsql_disconnect(conn);
	libsql_close(db);
	
}
