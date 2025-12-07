#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include <string>

struct Document {
    int id;
    int ownerId;           // student or other entity
    std::string docType;   // e.g. ADMISSION_DOC, PMD_ATTACHMENT
    std::string fileUrl;
    std::string fileHash;
    int uploadedBy;
    std::string uploadedAt;
};

#endif // DOCUMENT_HPP