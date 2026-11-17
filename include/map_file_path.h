/* MAP_FILE_PATH.H: the file path a map name stands for (0x214db0) and a
   buffer that path is printed into */

#ifndef MAP_FILE_PATH_H
#define MAP_FILE_PATH_H

/* a map name, and the file path it stands for */
struct s_map_name
{
	char text[256];
	void map_file_path_get(char *path) const;
};

/* a buffer a map name's file path is printed into */
struct s_map_file_path
{
	char text[256];
	void get(char const *map_name) { ((s_map_name const *)map_name)->map_file_path_get(text); }
};

#endif
