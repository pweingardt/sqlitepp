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

#include <iostream>

namespace sqlitepp {

    SQLiteException StatementNotPrepared("The statement has not been prepared or it has been finalized.");

    inline void Statement::checkPrepared() const {
        if(this->finalized) {
            throw StatementNotPrepared;
        }
    }

    Statement::Statement(Database& database) : db(database) {
        if(!db.isOpen()) {
            throw DatabaseNotOpened;
        }

        this->finalized = true;
        this->statement = NULL;
    }

    void Statement::exec() {
        this->checkPrepared();

        this->step();
        this->finalize();
    }

    void Statement::prepare(const std::string& str) {
        if(!this->finalized && this->statement) {
            std::cout << "The statement has not been finalized! Memory leak!" << std::endl;
        }

        this->lastResult = sqlite3_prepare_v2(this->db.database, str.c_str(), str.size(),
                &this->statement, NULL);


        if(this->lastResult == SQLITE_OK) {
            this->finalized = false;
        } else {
            throw SQLiteException(this->db.database);
        }
    }

    StepValue Statement::step() {
        this->checkPrepared();

        this->lastResult = sqlite3_step(this->statement);

        switch(this->lastResult) {
            case SQLITE_DONE:
                return DONE;

            case SQLITE_ROW:
                if(this->columns.size() == 0) {
                    for(int index = 0;
                            index < sqlite3_column_count(this->statement);
                            ++index) {
                        const char*p = (const char*)
                            sqlite3_column_name(this->statement, index);
                        std::string str(p);
                        this->columns.insert(this->columns.begin(),
                            std::pair<std::string, int>(p, index));
                    }
                }
                return ROW;

            default:
                return UNKNOWN;
        }
    }

    bool Statement::fetchRow(void) {
        this->checkPrepared();

        return (this->step() == ROW);
    }

    int Statement::getInt(const std::string& name) const {
        this->checkPrepared();

        if(this->columns.find(name) == this->columns.end()) {
            std::cout << "Unknown column " << name << std::endl;
        }
        return this->getInt(this->columns.find(name)->second);
    }

    int Statement::getInt(const int index) const {
        this->checkPrepared();

        return sqlite3_column_int(this->statement, index);
    }

    void Statement::getInt(const int index, int& out) const {
        this->checkPrepared();

        out = this->getInt(index);
    }

    std::string Statement::getString(const int index) const {
        this->checkPrepared();

        const char* p = (const char*)sqlite3_column_text(this->statement, index);
        if(p) {
            return std::string(p);
        } else {
            return "NULL";
        }
    }

    std::string Statement::getString(const std::string& name) const {
        this->checkPrepared();

        if(this->columns.find(name) == this->columns.end()) {
            std::cout << "Unknown column " << name << std::endl;
        }
        return this->getString(this->columns.find(name)->second);
    }

    void Statement::getString(const int index, std::string& out) const {
        this->checkPrepared();

        out = this->getString(index);
    }

    double Statement::getDouble(const std::string& name) const {
        this->checkPrepared();

        if(this->columns.find(name) == this->columns.end()) {
            std::cout << "Unknown column " << name << std::endl;
        }
        return this->getDouble(this->columns.find(name)->second);
    }

    double Statement::getDouble(const int index) const {
        this->checkPrepared();

        return sqlite3_column_double(this->statement, index);
    }

    void Statement::getDouble(const int index, double& out) const {
        this->checkPrepared();

        out = this->getDouble(index);
    }

    void Statement::bindInt(const int index, const int value) {
        this->checkPrepared();

        this->lastResult = sqlite3_bind_int(this->statement, index, value);
    }

    void Statement::bindString(const int index, const std::string& value) {
        this->checkPrepared();

        this->lastResult = sqlite3_bind_text(this->statement, index, value.c_str(), value.size(), NULL);
    }

    void Statement::bindDouble(const int index, const double value) {
        this->checkPrepared();

        this->lastResult = sqlite3_bind_double(this->statement, index, value);
    }

    void Statement::bindNull(const int index) {
        this->checkPrepared();

        this->lastResult = sqlite3_bind_null(this->statement, index);
    }

    void Statement::finalize(void) {
        if(this->statement && !this->finalized) {
            sqlite3_finalize(this->statement);
            this->columns.clear();
            this->statement = NULL;
            this->finalized = true;
        }
    }

    Statement::~Statement() {
        this->finalize();
    }

}
