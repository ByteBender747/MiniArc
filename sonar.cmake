# The sound effects are generated using my sonar cli tool
# You can find the it here: https://github.com/ByteBender747/sonar.git

function(sonar_generate_wave sfxFile)
    add_custom_command(
            OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Assets/${sfxFile}.wav
            COMMAND sonar
                -i ${CMAKE_CURRENT_SOURCE_DIR}/sfx/${sfxFile}.xml
                -b${WAVE_BITS}
                -s${WAVE_SAMPLE_FREQ}
                output:${CMAKE_CURRENT_SOURCE_DIR}/Assets/${sfxFile}.wav
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/sfx/${sfxFile}.xml
            COMMENT "Generating wave for ${sfxFile}"
    )
    add_custom_target(${sfxFile}_wav ALL
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Assets/${sfxFile}.wav
    )
endfunction()