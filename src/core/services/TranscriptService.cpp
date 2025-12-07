#include "TranscriptService.hpp"
#include <sstream>

std::string TranscriptService::buildTranscriptJson(int studentId) {
    auto studentOpt = studentRepository.getById(studentId);
    if (!studentOpt.has_value()) {
        return "{}";
    }
    Student s = studentOpt.value();

    auto enrollmentsWithCourses = enrollmentRepository.getByStudentIdWithCourses(studentId);

    std::stringstream ss;
    ss << "{";

    // Student section
    ss << "\"student\":{"
       << "\"id\":" << s.id << ","
       << "\"code\":\"" << s.studentCode << "\","
       << "\"name\":\"" << s.firstName << " " << s.lastName << "\","
       << "\"program\":\"" << s.program << "\""
       << "},";

    // Enrollments section
    ss << "\"enrollments\":[";
    bool first = true;
    for (const auto& pair : enrollmentsWithCourses) {
        const Course& c = pair.first;
        const Enrollment& e = pair.second;

        if (!first) {
            ss << ",";
        }
        first = false;

        ss << "{"
           << "\"courseCode\":\"" << c.courseCode << "\","
           << "\"title\":\"" << c.title << "\","
           << "\"credits\":" << c.credits << ","
           << "\"year\":" << e.year << ","
           << "\"semester\":\"" << e.semester << "\","
           << "\"grade\":" << (e.grade.has_value() ? std::to_string(e.grade.value()) : "null")
           << "}";
    }
    ss << "]";

    ss << "}";

    return ss.str();
}

bool TranscriptService::generateAndStoreTranscript(int studentId, const std::string& pdfUrl) {
    std::string json = buildTranscriptJson(studentId);
    if (json == "{}") {
        return false;
    }
    return transcriptRepository.insertTranscript(studentId, json, pdfUrl);
}