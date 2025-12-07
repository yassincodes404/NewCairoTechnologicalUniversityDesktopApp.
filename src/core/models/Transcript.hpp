#ifndef TRANSCRIPT_HPP
#define TRANSCRIPT_HPP

#include <string>

struct Transcript {
    int id;
    int studentId;
    std::string generatedAt;
    std::string contentJson;  // JSON snapshot of academic history
    std::string pdfUrl;       // optional link to exported PDF
};

#endif // TRANSCRIPT_HPP