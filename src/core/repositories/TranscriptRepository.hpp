#ifndef TRANSCRIPT_REPOSITORY_HPP
#define TRANSCRIPT_REPOSITORY_HPP

#include "../Database.hpp"
#include "../models/Transcript.hpp"
#include <vector>

class TranscriptRepository {
public:
    explicit TranscriptRepository(Database& db) : database(db) {}

    bool insertTranscript(int studentId, const std::string& contentJson, const std::string& pdfUrl);
    std::vector<Transcript> getByStudent(int studentId);

private:
    Database& database;
    Transcript rowToTranscript(int argc, char** argv, char** colNames);
};

#endif // TRANSCRIPT_REPOSITORY_HPP