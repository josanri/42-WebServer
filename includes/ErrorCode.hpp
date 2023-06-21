#ifndef HTTP_ERROR_CODE_HPP
# define HTTP_ERROR_CODE_HPP

/**
 * @brief 100 - Informative responses
 * 
 */

# define RESPONSE_CODE__CONTINUE_NUM		100;
# define RESPONSE_CODE__CONTINUE_MSG		"Continue";

/**
 * @brief 200 - Satisfactory responses
 * 
 */

# define RESPONSE_CODE__OK_NUM				200;
# define RESPONSE_CODE__OK_MSG				"OK";

# define RESPONSE_CODE__CREATED_NUM			201;
# define RESPONSE_CODE__CREATED_MSG			"Created";

# define RESPONSE_CODE__ACCEPTED_NUM		202;
# define RESPONSE_CODE__ACCEPTED_MSG		"Accepted";

/**
 * @brief 300 - Redirections
 * 
 */

# define RESPONSE_CODE__FOUND_NUM			302;
# define RESPONSE_CODE__FOUND_MSG			"Found";

/**
 * @brief 400 - Client error
 * 
 */

# define RESPONSE_CODE__BAD_REQUEST_NUM		400;
# define RESPONSE_CODE__BAD_REQUEST_MSG		"Bad Request";

# define RESPONSE_CODE__NOT_FOUND_NUM		404;
# define RESPONSE_CODE__NOT_FOUND_MSG		"Not Found";

/**
 * @brief 500 - Server error
 * 
 */

#endif