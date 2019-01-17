#ifndef URL_COMFIGER_HPP
#define URL_COMFIGER_HPP

#include <QString>

extern QString s_login_url;
extern QString s_get_data_list_url;
extern QString s_download_data_url;
extern QString s_upload_data_url;
extern QString s_get_file_list_url;
extern QString s_download_file_url;

void loadUrlConfigureFile(const QString &filename);

#endif // URL_COMFIGER_HPP
