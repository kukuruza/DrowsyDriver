
DEFINES += KYLE_MODEL_PATH=\\\"/Users/evgenytoropov/Documents/Xcode/DrowsyDriver/DrowsyDriver_2.2/FaceTracker\\\" \
           OPENCV_DATA_PATH=\\\"/usr/local/share/OpenCV\\\" \
           CLASSIFIER_FACE_PATH=\\\"/Users/evgenytoropov/Documents/Xcode/DrowsyDriver/DrowsyDriver_2.2/classifier_model\\\"

INCLUDEPATH += FaceTracker/include/
QMAKE_CXXFLAGS += -fpermissive

SOURCES += \
    CVmanager.cpp \
    Trigger.cpp \
    TrackerCompressive.cpp \
    TrackerOpticalFlow.cpp \
    TrackerCamshift.cpp \
    TrackerKyle.cpp \
    DetectorOpencv.cpp \
    DetectorKyleFace.cpp \
    Common.cpp \
    classifier_face/cluster_computation.cpp \
    classifier_face/feature_extractor.cpp \
    classifier_face/svm_classifier_face.cpp \
    classifier_face/histogram_generator_face.cpp \
    classifier_face/classifier_interface_face.cpp \
    Recognizer.cpp \
    classifier_eyes/classifier_interface_eyes.cpp \
    classifier_eyes/histogram_generator_eyes.cpp \
    classifier_eyes/svm_classifier_eyes.cpp \
    KyleGeneric.cpp \
    evgUtilities/timers.cpp \
    evgUtilities/mediaLoadSave.cpp \
    evgUtilities/rectOperations.cpp \
    FaceTracker/src/lib/Tracker.cc \
    FaceTracker/src/lib/PDM.cc \
    FaceTracker/src/lib/PAW.cc \
    FaceTracker/src/lib/Patch.cc \
    FaceTracker/src/lib/IO.cc \
    FaceTracker/src/lib/FDet.cc \
    FaceTracker/src/lib/FCheck.cc \
    FaceTracker/src/lib/CLM.cc

HEADERS  += \
    TrackingManager.h \
    CVmanager.h \
    TriggerManager.h \
    Trigger.h \
    TrackerGeneric.h \
    TrackerCompressive.h \
    TrackerOpticalFlow.h \
    TrackerCamshift.h \
    TrackerKyle.h \
    DetectingManager.h \
    DetectorKyleFace.h \
    DetectorOpencv.h \
    DetectorGeneric.h \
    Common.h \
    classifier_face/feature_extractor.h \
    classifier_face/cluster_computation.h \
    classifier_face/svm_classifier_face.h \
    classifier_face/classifier_interface_face.h \
    classifier_face/histogram_generator_face.h \
    Recognizer.h \
    classifier_eyes/classifier_interface_eyes.h \
    classifier_eyes/histogram_generator_eyes.h \
    classifier_eyes/svm_classifier_eyes.h \
    KyleGeneric.h \
    evgUtilities/timers.h \
    evgUtilities/mediaLoadSave.h \
    evgUtilities/rectOperations.h \
    FaceTracker/include/FaceTracker/Tracker.h \
    FaceTracker/include/FaceTracker/PDM.h \
    FaceTracker/include/FaceTracker/PAW.h \
    FaceTracker/include/FaceTracker/Patch.h \
    FaceTracker/include/FaceTracker/IO.h \
    FaceTracker/include/FaceTracker/FDet.h \
    FaceTracker/include/FaceTracker/FCheck.h \
    FaceTracker/include/FaceTracker/CLM.h

LIBS += -lopencv_core\
        -lopencv_highgui\
        -lopencv_ml\
        -lopencv_objdetect\
        -lopencv_imgproc\
        -lopencv_video\
        -lopencv_flann\
        -lboost_thread\
        -lboost_filesystem\
        -lboost_system\
        -lvl
