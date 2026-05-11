add_executable(Serwer Server/server.cpp)
add_executable(Klient Client/client.cpp)

add_custom_command(TARGET Klient POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/Client/moneyData.txt"
        $<TARGET_FILE_DIR:Klient>
)

add_custom_command(TARGET Serwer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/Server/ticketsData.txt"
        $<TARGET_FILE_DIR:Serwer>
)