
########################################################
# Files

SET (bubbles_SRCS
     bubbles.cpp
     bubblesgui.cpp
     bblmarkersymbollayer.cpp
     bblmarkersymbollayerwidget.cpp
)

SET (bubbles_UIS bubblesguibase.ui
	bubbleswidgetbase.ui
)

SET (bubbles_MOC_HDRS
     bubbles.h
     bubblesgui.h
     bblmarkersymbollayerwidget.h
)

SET (bubbles_RCCS  bubbles.qrc)

########################################################
# Build

QT4_WRAP_UI (bubbles_UIS_H  ${bubbles_UIS})

QT4_WRAP_CPP (bubbles_MOC_SRCS  ${bubbles_MOC_HDRS})

QT4_ADD_RESOURCES(bubbles_RCC_SRCS ${bubbles_RCCS})

ADD_LIBRARY (bubblesplugin MODULE ${bubbles_SRCS} ${bubbles_MOC_SRCS} ${bubbles_RCC_SRCS} ${bubbles_UIS_H})

INCLUDE_DIRECTORIES(
     ${CMAKE_CURRENT_SOURCE_DIR}
	 ${PROJ_INCLUDE_DIR}
     ${CMAKE_CURRENT_BINARY_DIR}
	 ${CMAKE_CURRENT_BINARY_DIR}/../../ui
    ../../app
     ../../core
     ../../core/geometry
     ../../core/raster
     ../../core/symbology-ng
     ../../core/auth
     ../../gui/symbology-ng
     ../../gui
	 ../../gui/editorwidgets
	 ../../ui
     ../../core/dxf
     ..
)

TARGET_LINK_LIBRARIES(bubblesplugin
  qgis_core
  qgis_gui
)


########################################################
# Install

INSTALL(TARGETS bubblesplugin
  RUNTIME DESTINATION ${QGIS_PLUGIN_DIR}
  LIBRARY DESTINATION ${QGIS_PLUGIN_DIR})

