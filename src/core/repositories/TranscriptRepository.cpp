#include "TranscriptRepository.hpp"
#include <sstream>

bool TranscriptRepository::insertTranscript(int studentId, const std::string& contentJson, const std::string& pdfUrl) {
    std::stringstream sql;
    sql << "INSERT INTO transcripts (student_id, content, pdf_url) VALUES ("
        << studentId << ", "
        << "'" << contentJson << "', "
        << "'" << pdfUrl << "');";
    return database.executeNonQuery(sql.str());
}

std::vector<Transcript> TranscriptRepository::getByStudent(int studentId) {
    std::vector<Transcript> transcripts;
    std::stringstream sql;
    sql << "SELECT id, student_id, generated_at, content, pdf_url "
        << "FROM transcripts WHERE student_id = " << studentId << " ORDER BY generated_at DESC;";

    database.executeQuery(sql.str(), [&transcripts, this](void*, int argc, char** argv, char** colNames) -> int {
        transcripts.push_back(rowToTranscript(argc, argv, colNames));
        return 0;
    });

    return transcripts;
}

Transcript TranscriptRepository::rowToTranscript(int argc, char** argv, char** colNames) {
    Transcript t{};
    for (int i = 0; i < argc; ++i) {
        std::string colName = colNames[i];
        std::string value = argv[i] ? argv[i] : "";

        if (colName == "id") {
            t.id = std::stoi(value);
        } else if (colName == "student_id") {
            t.studentId = std::stoi(value);
        } else if (colName == "generated_at") {
            t.generatedAt = value;
        } else if (colName == "content") {
            t.contentJson = value;
        } else if (colName == "pdf_url") {
            t.pdfUrl = value;
        }
    }
    return t;
}