/* Copyright (C)
 * 2012 - Paul Weingardt
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef SQLITEPP_H
#define SQLITEPP_H

#include <sqlite3.h>
#include <time.h>
#include <stdlib.h>
#include <map>
#include <string>

namespace sqlitepp {

    /**
     * @brief converts a double into a string
     */
    std::string doubleToString(const double value);

    /**
     * @brief converts a float into a string
     */
    std::string floatToString(const float value);

    /**
     * @brief converts an integer into a string
     * @return
     */
    std::string intToString(const int value);

    class Statement;

    /**
     * @brief used for transactions
     */
    enum TransactionFlags {DEFERRED, IMMEDIATE, EXCLUSIVE};

    /**
     * @brief returned from step()
     *
     * ROW = a new row is available
     * DONE = there are no more results available
     * UNKNOWN = unknown
     */
    enum StepValue {ROW, DONE, UNKNOWN};

    /**
     * @brief Indicates the mode. This means, that
     * you still can create an insert statement and
     * execute it, but an error will occur.
     */
    enum OpenFlags {READONLY, READWRITE, CREATE};


    /**
     * @brief The main database class
     */
    class Database {
        friend class Statement;
        private:
            /**
             * @brief initializes all fields
             */
            void init(void);

            /**
             * @brief indicates, if a database has been opened
             */
            bool isopen;

            /**
             * @brief indicates, if there is an active transaction
             */
            bool transaction;

            /**
             * @brief the sqlite3* pointer
             */
            sqlite3* database;

            /**
             * @brief the last result from sqlite3
             */
            int lastResult;

            inline void checkDatabaseOpened() const;
        public:
            /**
             * @brief empty constructor. Does not open a database
             */
            Database(void);

            /**
             * @brief Opens a sqlite3 database with the passed flag
             *
             * @param path
             * @param flag
             */
            Database(const std::string& path, const OpenFlags flag = CREATE);

            /**
             * @brief destructor
             */
            ~Database(void);

            /**
             * @brief Opens a database with the passed flag
             *
             * @param path
             * @param flag
             */
            void open(const std::string& path, const OpenFlags flag = CREATE);

            /**
             * @brief returns true, when a database has been opened, false otherwise
             */
            bool isOpen(void);

            /**
             * @brief closes the database
             */
            void close(void);

            /**
             * @brief activates foreign keys
             */
            void activateForeignKeys(void);

            /**
             * @brief deactivates foreign keys
             */
            void deactivateForeignKeys(void);

            /**
             * @brief executes a single statement without return values
             *
             * @param sql SQL statement
             */
            int exec(const std::string& sql);

            /**
             * @brief Begins a transaction
             */
            void beginTransaction(const TransactionFlags = DEFERRED);

            /**
             * @brief commits a transaction
             */
            void endTransaction(void);

            /**
             * @brief executes a rollback on the current transaction
             */
            void rollback(void);

            /**
             * @brief gets the last ID, that has been inserted
             *
             * @return
             */
            int getLastRowId(void);
    };


    /**
     * @brief A prepared statement
     */
    class Statement {
        private:
            /**
             * @brief the last result returned by sqlite
             */
            int lastResult;

            /**
             * @brief true if the statement has been finalized, false otherwise.
             *
             * also used to indicate, whether a statement has been prepared:
             * prepared = !finalized
             */
            bool finalized;

            /**
             * @brief the sqlite3 statement
             */
            sqlite3_stmt* statement;

            /**
             * @brief the database reference
             */
            Database& db;

            /**
             * @brief the columns, that has been selected, and their indices
             */
            std::map<std::string, int> columns;

            /**
             * @brief performs a single step
             */
            StepValue step(void);

            /**
             * @brief checks, if a statement has been prepared. Throws an exception, if not.
             */
            inline void checkPrepared() const;

        public:
            /**
             * @brief Default constructor
             *
             * @param db database, on which the statement is executed
             */
            Statement(Database& db);

            /**
             * @brief Destructor
             */
            ~Statement();

            /**
             * @brief Binds the nth parameter with the passed value as string
             *
             * @param n
             * @param value
             */
            void bindString(const int n, const std::string& value);

            /**
             * @brief Binds the nth parameter with the passed value as integer
             *
             * @param n
             * @param value
             */
            void bindInt(const int n, const int value);

            /**
             * @brief Binds the nth parameter with the passed value as integer
             *
             * @param n
             * @param value
             */
            void bindDouble(const int n, const double value);

             /**
             * @brief Binds the nth parameter with the null value
             *
             * @param n
             */
            void bindNull(const int n);

            /**
             * @brief gets a column as string
             *
             * @param column
             *
             * @return
             */
            std::string getString(const std::string& column) const;

            /**
             * @brief gets a column as integer
             *
             * @param column
             *
             * @return
             */
            int getInt(const std::string& column) const;

            /**
             * @brief gets a column as double
             *
             * @param column
             *
             * @return
             */
            double getDouble(const std::string& column) const;

            /**
             * @brief gets the nth column as string and saves it in out
             *
             * @param n the index
             * @param out the output string
             */
            void getString(const int n, std::string& out) const;

            /**
             * @brief gets the nth column as integer and saves it in out
             *
             * @param n the index
             * @param out the output integer
             */
            void getInt(const int n, int& out) const;

            /**
             * @brief gets the nth column as string and saves it in out
             *
             * @param n the index
             * @param out the output double
             */
            void getDouble(const int n, double& out) const;

            /**
             * @brief gets the nth column as string
             *
             * @param n
             */
            std::string getString(const int n) const ;

            /**
             * @brief gets the nth column as integer
             *
             * @param n
             */
            int getInt(const int n) const;

            /**
             * @brief gets the nth column as double
             *
             * @param n
             */
            double getDouble(const int n) const;

            /**
             * @brief prepares a string as statement
             *
             * @param sql SQL statement
             */
            void prepare(const std::string& sql);

            /**
             * @brief fetches the next row
             *
             * @return
             */
            bool fetchRow(void);

            /**
             * @brief executes the prepared statement
             */
            void exec(void);

            /**
             * @brief releases any allocated resources. Use it, when you want to
             * reuse a statement object.
             */
            void finalize(void);
    };


    class SQLiteException : public std::exception {
        private:
            std::string error;

        public:
            /**
             * @brief Constructs a SQLiteException with the passed error message
             */
            SQLiteException(const std::string& str) {
                this->error = str;
            }

            /**
             * @brief The error message is the last sqlite error in the
             * passed sqlite database.
             *
             * @param db
             */
            SQLiteException(sqlite3* db) {
                this->error = std::string(sqlite3_errmsg(db));
            }

            virtual ~SQLiteException() throw() {
            }

            /**
             * @brief gets the error message
             */
            virtual const char* what() const throw() {
                return this->error.c_str();
            }
    };

    extern SQLiteException DatabaseNotOpened;
    extern SQLiteException DatabaseOpened;
    extern SQLiteException StatementNotPrepared;
}

#endif
