/*
 * metainfo.h
 *
 *  Created on: 2014年3月30日
 *      Author: Feng
 */

#ifndef __METAINFO_H__
#define __METAINFO_H__

#include <common/common.h>

namespace cires {

struct ImageMetaInfo
{
	typedef unsigned int id_type;
	const static id_type nindex = static_cast<id_type>(-1);

	id_type  id;
	string   name;
	size     origin_size;
	size     norm_size;

	ImageMetaInfo()
		: id(nindex)
	{}

	ImageMetaInfo( id_type i,
				   const string& n = "",
				   const Mat image = Mat() )
		: id(i), name(n)
		, origin_size(image.size())
		, norm_size(image.size())
	{}

	ImageMetaInfo( const string& n ,
				   const Mat& image = Mat(),
				   const size& ns = size())
		: id(-1), name(n)
		, origin_size(image.size())
		, norm_size(ns)
	{}
};


class ImageMetaInfoIndex
{
	public:
		typedef ImageMetaInfo          node_type;
		typedef ImageMetaInfo::id_type id_type;

		static const id_type nindex = ImageMetaInfo::nindex;

		node_type get_node_by_id( id_type id ) const;
		node_type get_node_by_name( const string& name ) const;
		id_type get_id_by_name( const string& name ) const;
		string get_name_by_id( id_type id ) const;

		template <typename ...Args>
		id_type insert( Args... args )
		{
			return insert(node_type(forward<Args>(args)...));
		}
		id_type insert( const node_type& n );

	private:
		unordered_map<string, node_type> m_node_indexer;
		vector<string>                   m_name_store;
		deque<id_type>                   m_unused_ids;
};

}
#endif /* __METAINFO_H__ */
