#include "libsql.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	libsql_database_t db;
	libsql_connection_t conn;
	const char *err = NULL;
	int retval = 0;
	libsql_rows_t rows;
	libsql_row_t row;
	int num_cols;

  char* url= getenv ("LIBSQL_URL");
  if (!url)
		url=":memory:";	
  fprintf(stderr, "url=%s\n", url);
  retval = libsql_open_ext(url, &db, &err);
  //retval = libsql_open_remote(toStringz(url),toStringz(""), &db, &err);	
  if (retval != 0) {
		fprintf(stderr, "%s\n", err);
		goto quit;
	}

  retval = libsql_connect(db, &conn, &err);
  if (retval != 0) {
		fprintf(stderr, "%s\n", err);
		goto quit;
	}

 const char* create_table="CREATE TABLE Persons2(Name TEXT,Age INTEGER);";

  retval = libsql_execute(conn,create_table, &err);
  if (retval != 0) {
		fprintf(stderr, "%s\n", err);
		goto quit;
	}

	const char* insert_person="INSERT INTO Persons2 VALUES ('Paul',20);";

	retval = libsql_execute(conn,insert_person, &err);
  if (retval != 0) {
		fprintf( stderr, "%s\n", err);
		goto quit;
	}
  
 const char* insert_person2="INSERT INTO Persons2 VALUES ('Laura',30)";

	retval = libsql_execute(conn,insert_person2, &err);
  if (retval != 0) {
		fprintf( stderr, "%s\n", err);
	}
  assert(retval == 0);

	retval = libsql_query(conn, "SELECT * FROM Persons2;", &rows, &err);
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
 		 assert(retval == 0);
		}
		err = NULL;
	}
 assert(retval == 0);

 quit:
	libsql_free_rows(rows);
	libsql_disconnect(conn);
	libsql_close(db);
}
