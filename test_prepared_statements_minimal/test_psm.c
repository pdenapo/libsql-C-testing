#include "libsql.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

libsql_stmt_t insert_stmt;

int insert_person(const char* name,const char **out_err_msg)
{
		fprintf(stderr,"insert_person (%s)\n",name);
		int retval=libsql_bind_string(insert_stmt,1, name, out_err_msg);
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


  retval = libsql_open_ext(":memory:", &db, &err);
  if (retval != 0) {
		fprintf(stderr, "%s\n", err);
		goto quit;
	}

  retval = libsql_connect(db, &conn, &err);
  if (retval != 0) {
		fprintf(stderr, "%s\n", err);
		goto quit;
	}
	
 const char* create_table="CREATE TABLE Persons(Name TEXT);";
  fprintf(stderr, "%s\n", create_table);
  retval = libsql_execute(conn,create_table, &err);
  if (retval != 0) {
		fprintf(stderr, "%s\n", err);
		goto quit;
	}

	const char* insert_sql = "INSERT INTO Persons VALUES (?)";
		
	retval=libsql_prepare(conn, insert_sql, &insert_stmt, &err);
	if (retval != 0) {
		fprintf( stderr, "%s\n", err);
		goto quit;
	}

	retval= insert_person("Paul",&err);
	if (retval != 0) {
		fprintf( stderr, "%s\n", err);
		goto quit;
	}
	retval= insert_person("Laura",&err);
	if (retval != 0) {
		fprintf( stderr, "%s\n", err);
		goto quit;
	}

  retval = libsql_query(conn, "SELECT * FROM Persons;", &rows, &err);
	if (retval != 0) {
		fprintf( stderr, "%s\n", err);
		goto quit;
	}

	while ((retval = libsql_next_row(rows, &row, &err)) == 0) {
				if (!err && !row) {
					break;
				}
			  const char* value;
				retval = libsql_get_string(row,0 , &value, &err);
				if (retval != 0) {
					fprintf(stderr, "%s\n", err);
				} else {
					printf("%s\n", value);
				}
				
		}
 quit:
	if (insert_stmt)	libsql_free_stmt(insert_stmt);
	libsql_free_rows(rows);
	libsql_disconnect(conn);
	libsql_close(db);
	
}
