o2physics_add_dpl_workflow(my-first-task
                  SOURCES tasks/myFirstTask.cxx
                  PUBLIC_LINK_LIBRARIES O2::Framework
                  COMPONENT_NAME Analysis)
