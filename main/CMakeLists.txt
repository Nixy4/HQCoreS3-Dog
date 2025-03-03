idf_component_register(SRCS 
				"esp32-dog.cpp"
				"app_led.c"
				"app_speech_queue.c"
				"app_speech_wakeup.c"
				"app_speech_recsrc.c"
				"wav_player.c"
				"file_manager.c"
				"page_wakeup.c"
                    INCLUDE_DIRS ".")

component_compile_options(-fpermissive)

spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)