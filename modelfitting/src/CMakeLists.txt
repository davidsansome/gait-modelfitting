# Source files
SET(MODELFITTING-SOURCES
	main.cpp
	mainwindow.cpp
	glview.cpp
	frameinfo.cpp
	../filters/thighmodel.cpp
	model.cpp
	modelloader.cpp
	types.cpp
	mapreduceprogress.cpp
	graphplotter.cpp
	params.cpp
	batchmode.cpp
	energyplotter.cpp
	timeplotter.cpp
	errorcorrection.cpp
	fftplotter.cpp
	curvefitter.cpp
	fft.cpp
	classifydialog.cpp
	people.cpp
	listmodels.cpp
	signature.cpp
	shader.cpp
)

# Header files that have Q_OBJECT in
SET(MODELFITTING-MOC-HEADERS
	mainwindow.h
	glview.h
	frameinfo.h
	mapreduceprogress.h
	graphplotter.h
	errorcorrection.h
	batchmode.h
	classifydialog.h
)

# UI files
SET(MODELFITTING-UI
	mainwindow.ui
	mapreduceprogress.ui
	graphplotter.ui
	errorcorrection.ui
	classifydialog.ui
)

# Resource files
SET(MODELFITTING-RESOURCES
	../data/resources.qrc
	../shaders/shaders.qrc
)

SET (EXECUTABLE_OUTPUT_PATH ..)

QT4_WRAP_CPP(MODELFITTING-SOURCES-MOC ${MODELFITTING-MOC-HEADERS})
QT4_WRAP_UI(MODELFITTING-SOURCES-UI ${MODELFITTING-UI})
QT4_ADD_RESOURCES(MODELFITTING-SOURCES-RESOURCE ${MODELFITTING-RESOURCES})

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../vis4d
	${CMAKE_CURRENT_SOURCE_DIR}/../common
	${V4D_INCLUDE_DIR}
	${CG_INCLUDE_PATH}
	${OPENGL_INCLUDE_DIR}
)

LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/../vis4d)
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/../common)

ADD_EXECUTABLE(modelfitting ${MODELFITTING-SOURCES} ${MODELFITTING-SOURCES-MOC} ${MODELFITTING-SOURCES-UI} ${MODELFITTING-SOURCES-RESOURCE})

TARGET_LINK_LIBRARIES(modelfitting
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTOPENGL_LIBRARY}
	-lV4D
	-lsvl
	${ZLIB_LIBRARY}
	-lcommon
	${OPENGL_LIBRARIES}
	${FFTWD_LIB}
	-mwindows
)

INSTALL(TARGETS modelfitting
	RUNTIME DESTINATION bin)


