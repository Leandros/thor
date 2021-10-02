#ifndef LOLPACK_PARSER_H
#define LOLPACK_PARSER_H

/*!
 * \brief Parse the config file, validate it and generate the prepared map
 *        containing the full path and hash of each file.
 * \param cfg Path to config file.
 * \param basepath The path all paths in the config file are relative to.
 * \param out Pointer to un-initialized vector.
 * \return Zero on success, non-zero on failure.
 */
int
parse_files(char const *cfg, char const *basepath, struct vec_file *out);

#endif /* LOLPACK_PARSER_H */

