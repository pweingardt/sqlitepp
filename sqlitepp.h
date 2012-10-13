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

    std::string doubleToString(const double value);
    std::string floatToString(const float value);
    std::string intToString(const int value);

    class Statement;

    enum TransactionFlags {DEFERRED, IMMEDIATE, EXCLUSIVE};
    enum StepValue {ROW, DONE, UNKNOWN};
    enum OpenFlags {READONLY, READWRITE, CREATE};

    class Database {
        friend class Statement;
        private:
            void init(void);

            bool isopen, transaction;
            sqlite3* database;
            int lastResult;

        public:
            Database(void);
            Database(const std::string&, const OpenFlags = CREATE);
            ~Database(void);

            void open(const std::string&, const OpenFlags = CREATE);
            bool isOpen(void);
            void close(void);

            void activateForeignKeys(void);
            void deactivateForeignKeys(void);

            int exec(const std::string&);

            void beginTransaction(const TransactionFlags = DEFERRED);
            void endTransaction(void);
            void rollback(void);

            int getLastRowId(void);
    };


    class Statement {
        private:
            int lastResult;
            bool finalized;
            sqlite3_stmt* statement;
            Database& db;

            std::map<std::string, int> columns;

            StepValue step(void);

        public:
            Statement(Database&);
            ~Statement();

            void bindString(const int, const std::string&);
            void bindInt(const int, const int);
            void bindDouble(const int, const double);
            void bindNull(const int);

            std::string getString(const std::string&) const;
            int getInt(const std::string&) const;
            double getDouble(const std::string&) const;

            void getString(const int, std::string&) const;
            void getInt(const int, int&) const;
            void getDouble(const int, double&) const;

            std::string getString(const int) const ;
            int getInt(const int) const;
            double getDouble(const int) const;

            void prepare(const std::string&);
            bool fetchRow(void);
            void exec(void);
            void finalize(void);
    };


    class SQLiteException : public std::exception {
        private:
            sqlite3* database;

        public:
            SQLiteException(sqlite3* db) : database(db) {
            }

            virtual const char* what() const throw() {
                return sqlite3_errmsg(this->database);
            }
    };
}

#endif
