/*
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Please visit https://chensongpoixs.github.io for detail
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
 /*****************************************************************************
				   Author: chensong
				   date:  2025-10-17



 ******************************************************************************/

#include "utils/string_utils.h"

#include "rtc_base/string_encode.h"
namespace  gb_media_server
{
	namespace string_utils
	{
		namespace
		{
			std::string  FileNameExt(const std::string &path)
			{
				auto pos = path.find_last_of("/\\");
				if (pos != std::string::npos)
				{
					if (pos + 1 < path.size())
					{
						return path.substr(pos + 1);
					}
				}
				return path;
			}
		}
		size_t split(std::string source,
			char delimiter,
			std::vector<std::string>* fields)
		{
			//RTC_DCHECK(fields);
			fields->clear();
			size_t last = 0;
			for (size_t i = 0; i < source.length(); ++i) {
				if (source[i] == delimiter) {
					fields->emplace_back(source.substr(last, i - last));
					last = i + 1;
				}
			}
			fields->emplace_back(source.substr(last));
			return fields->size();
		}
		  std::string  GetSessionNameFromUrl(const std::string &url)
		{
			//webrtc://chensong.com:9091/live/test
			//webrtc://chensong.com:9091/domain/live/test 
			std::vector<std::string> list;
			split(url, '/', &list);
			if (list.size() < 5)
			{
				return "";
			}
			std::string domain, app, stream;
			if (list.size() == 5)
			{
				domain = list[2];
				app = list[3];
				stream = list[4];
			}
			else if (list.size() == 6)
			{
				domain = list[3];
				app = list[4];
				stream = list[5];
			}

			auto pos = domain.find_first_of(':');
			if (pos != std::string::npos)
			{
				domain = domain.substr(0, pos);
			}
			  
			return /*domain + "/" + */app + "/" + stream;
		}


		  std::string  FileExt(const std::string & path)
		  {
			  std::string filename = FileNameExt(path);
			  auto pos = filename.find_last_of(".");
			  if (pos != std::string::npos)
			  {
				  if (pos != 0)
				  {
					  return filename.substr(pos+1, filename.length()-1);
				  }
			  }
			  return filename;
		  }
	}
}
