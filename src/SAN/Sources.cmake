set(San_Src_Files
	${SAN_DIR}/cylindre.cpp
	${SAN_DIR}/ortho_cyl.cpp
)

SOURCE_GROUP(San FILES ${San_Src_Files})

set(Elise_Src_Files
	${Elise_Src_Files}
	${San_Src_Files}
)
