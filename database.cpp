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

#include "sqlitepp.h"

namespace sqlitepp {

    SQLiteException DatabaseNotOpened("Database has not been opened yet.");
    SQLiteException DatabaseOpened("A database has been opened already.");

    inline void Database::checkDatabaseOpened() const {
        if(!this->isopen) {
            throw DatabaseNotOpened;
        }
    }

    int Database::getLastRowId(void) {
        this->checkDatabaseOpened();

        return sqlite3_last_insert_rowid(this->database);
    }

    Database::Database(const std::string& file, const OpenFlags flags) {
        this->init();
        this->open(file, flags);
    }

    Database::Database(void) {
        this->init();
    }

    void Database::init(void) {
        this->isopen = this->transaction = false;
        this->database = NULL;
        this->lastResult = 0;
    }

    void Database::open(const std::string& file, const OpenFlags flags) {
        if(this->isopen) {
            throw DatabaseOpened;
        }

        int flag = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

        switch(flags) {
            case READONLY:
                flag = SQLITE_OPEN_READONLY;
                break;

            case READWRITE:
                flag = SQLITE_OPEN_READWRITE;
                break;

            case CREATE:
                flag = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
                break;
        }

        this->lastResult = sqlite3_open_v2(file.c_str(), &this->database, flag, NULL);
        if(this->lastResult != SQLITE_OK) {
            throw SQLiteException(this->database);
        } else {
            this->isopen = true;
        }
    }

    void Database::activateForeignKeys(void) {
        this->exec("PRAGMA foreign_keys = ON;");
    }

    void Database::deactivateForeignKeys(void) {
        this->exec("PRAGMA foreign_keys = OFF;");
    }

    bool Database::isOpen(void) {
        return this->isopen;
    }

    Database::~Database(void) {
        this->close();
    }

    int Database::exec(const std::string& str) {
        this->checkDatabaseOpened();

        this->lastResult = sqlite3_exec(this->database, str.c_str(), NULL, NULL, NULL);
        if(this->lastResult != SQLITE_OK) {
            throw SQLiteException(this->database);
        }

        return sqlite3_changes(this->database);
    }

    void Database::beginTransaction(const TransactionFlags flags) {
        // No need do check, if a database has been opened, exec does that

        if(this->transaction) {
            return;
        }

        std::string flag("DEFERRED");
        switch(flags) {
            case DEFERRED:
                flag = "DEFERRED";
                break;

            case IMMEDIATE:
                flag = "IMMEDIATE";
                break;

            case EXCLUSIVE:
                flag = "EXCLUSIVE";
                break;
        }
        std::string execStr = "BEGIN " + flag + " TRANSACTION;";

        exec(execStr);
        this->transaction = true;
    }

    void Database::rollback(void) {
        // No need do check, if a database has been opened, exec does that
        if(!this->transaction) {
            return;
        }

        this->exec("ROLLBACK;");
        this->transaction = false;
    }

    void Database::endTransaction(void) {
        // No need do check, if a database has been opened, exec does that
        if(!this->transaction) {
            return;
        }

        this->exec("END TRANSACTION;");
        this->transaction = false;
    }

    void Database::close(void) {
        if(this->isopen) {
            if(this->transaction) {
                this->rollback();
            }

            sqlite3_close(this->database);
            this->database = NULL;
            this->isopen = false;
            this->transaction = false;
        }
    }
}

