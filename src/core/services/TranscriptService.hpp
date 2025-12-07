#ifndef TRANSCRIPT_SERVICE_HPP
#define TRANSCRIPT_SERVICE_HPP

#include "../repositories/StudentRepository.hpp"
#include "../repositories/EnrollmentRepository.hpp"
#include "../repositories/CourseRepository.hpp"
#include "../repositories/TranscriptRepository.hpp"
#include "../models/Student.hpp"
#include "../models/Enrollment.hpp"
#include "../models/Course.hpp"
#include "../models/Transcript.hpp"
#include <string>
#include <vector>

class TranscriptService {
public:
    TranscriptService(StudentRepository& studentRepo,
                      EnrollmentRepository& enrollmentRepo,
                      CourseRepository& courseRepo,
                      TranscriptRepository& transcriptRepo)
        : studentRepository(studentRepo),
          enrollmentRepository(enrollmentRepo),
          courseRepository(courseRepo),
          transcriptRepository(transcriptRepo) {}

    std::string buildTranscriptJson(int studentId);
    bool generateAndStoreTranscript(int studentId, const std::string& pdfUrl);

private:
    StudentRepository& studentRepository;
    EnrollmentRepository& enrollmentRepository;
    CourseRepository& courseRepository;
    TranscriptRepository& transcriptRepository;
};

#endif // TRANSCRIPT_SERVICE_HPP