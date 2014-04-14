/*
 * metainfo.cpp
 *
 *  Created on: 2014年3月30日
 *      Author: Feng
 */

#include "metainfo.h"
using namespace cires;


ImageMetaInfoIndex::node_type ImageMetaInfoIndex::get_node_by_id( ImageMetaInfoIndex::id_type id ) const
{
	string name;
	if (id < 0 || id >= m_name_store.size() || (name = m_name_store[id]).empty())
		return node_type(nindex);
	return get_node_by_name(name);
}

ImageMetaInfoIndex::node_type ImageMetaInfoIndex::get_node_by_name( const string& name ) const
{
	auto const it = m_node_indexer.find(name);
	if (it == m_node_indexer.end())
		return node_type(nindex);
	else
		return it->second;
}

ImageMetaInfoIndex::id_type ImageMetaInfoIndex::get_id_by_name( const string& name ) const
{
	auto const it = m_node_indexer.find(name);
	if (it == m_node_indexer.end())
		return nindex;
	else
		return it->second.id;
}

string ImageMetaInfoIndex::get_name_by_id( ImageMetaInfoIndex::id_type id ) const
{
	string name;
	if (id < 0 || id >= m_name_store.size() || (name = m_name_store[id]).empty())
		return name;
	return name;
}

ImageMetaInfoIndex::id_type ImageMetaInfoIndex::insert( const ImageMetaInfoIndex::node_type& node )
{
	auto const it = m_node_indexer.find(node.name);
	if (it != m_node_indexer.end())
		return it->second.id;

	id_type id;
	if (m_unused_ids.empty()) {
		id = m_name_store.size();
		m_name_store.push_back(node.name);
	}
	else {
		id = m_unused_ids.back();
		m_unused_ids.pop_back();
		m_name_store[id] = node.name;
	}

	node_type dup_n = node;
	dup_n.id = id;
	m_node_indexer[dup_n.name] = dup_n;
	return id;
}
