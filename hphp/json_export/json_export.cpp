#include <iostream>
#include <vector>

#include "process-init.h"
#include "../util/json.h"
#include "../runtime/vm/repo-helpers.h"
#include "../runtime/vm/repo.h"

using namespace HPHP;

int main(int argc, const char* argv[]) {
    // Initialize process
    register_process_init();
    initialize_repo();
    init_thread_locals();

    if(argc < 3) {
        std::cerr << "Missing parameters!" << std::endl;
        std::cerr << "Please specify the location of the repo as the first argument." << std::endl;
        std::cerr << "Please specify the location of the resulting .json file as the second argument." << std::endl;
        return 1;
    }

    std::string repo_path(argv[1]);
    std::string output_path(argv[2]);

    HPHP::RuntimeOption::RepoLocalMode = "--";
    HPHP::RuntimeOption::RepoDebugInfo = true;
    HPHP::RuntimeOption::RepoJournal = "memory";
    HPHP::RuntimeOption::RepoEvalMode = "central";
    HPHP::RuntimeOption::EnableHipHopSyntax = true;
    HPHP::RuntimeOption::EnableZendCompat = false;
    HPHP::RuntimeOption::EvalJitEnableRenameFunction = false;

    // Turn off commits, because we don't want systemlib to get included
    HPHP::RuntimeOption::RepoCommit = false;

    HPHP::RuntimeOption::RepoCentralPath = repo_path;
    HPHP::Repo repo = HPHP::Repo::get();

    std::stringstream ssSelect;
    ssSelect << "SELECT f.path, f.md5"
             << " FROM " << repo.table(0, "FileMD5") << " AS f, "
             << repo.table(0, "Unit") << " AS u"
             << " WHERE f.md5 == u.md5 AND unitSn IN ("
             << " SELECT MAX(unitSn)"
             << " FROM " << repo.table(0, "FileMD5") << " AS f, "
             << repo.table(0, "Unit") << " AS u"
             << " WHERE f.md5 == u.md5"
             << " GROUP BY f.path"
             << " );";

    RepoStmt stmt(repo);
    stmt.prepare(ssSelect.str());

    std::ofstream output(output_path);
    HPHP::JSON::DocTarget::OutputStream out(output, 0);
    HPHP::JSON::DocTarget::MapStream mainObj(out);
    mainObj.add("units");
    HPHP::JSON::DocTarget::ListStream units(out);

    std::vector<std::string> paths;
    std::vector<MD5> checksums;
    RepoQuery q(stmt);
    do {
        q.step();

        if(q.row()) {
            const char* filepath;
            q.getText(0, filepath);
            paths.push_back(filepath);

            MD5 file_checksum;
            q.getMd5(1, file_checksum);
            checksums.push_back(file_checksum);
        }
    } while (!q.done());

    std::vector<std::string>::const_iterator path_it = paths.begin();
    std::vector<MD5>::const_iterator cs_it = checksums.begin();
    while(path_it != paths.end() || cs_it != checksums.end()) {
        units.next();

        Unit* u = repo.urp().load(*path_it, *cs_it);
        u->toJson(out);

        ++path_it;
        ++cs_it;
    }
    units.done();

    mainObj.done();

    return 0;
}

