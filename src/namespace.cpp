/*
	Client library for mastermind
	Copyright (C) 2013-2014 Yandex

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "namespace_p.hpp"

namespace mastermind {

namespace_settings_t::data::data()
	: redirect_expire_time(0)
	, redirect_content_length_threshold(-1)
	, is_active(false)
	, can_choose_couple_to_upload(false)
	, multipart_content_length_threshold(0)
{
}

namespace_settings_t::data::data(const namespace_settings_t::data &d)
	: name(d.name)
	, groups_count(d.groups_count)
	, success_copies_num(d.success_copies_num)
	, auth_key(d.auth_key)
	, static_couple(d.static_couple)
	, auth_key_for_write(d.auth_key_for_write)
	, auth_key_for_read(d.auth_key_for_read)
	, sign_token(d.sign_token)
	, sign_path_prefix(d.sign_path_prefix)
	, sign_port(d.sign_port)
	, redirect_expire_time(d.redirect_expire_time)
	, redirect_content_length_threshold(d.redirect_content_length_threshold)
	, is_active(d.is_active)
	, can_choose_couple_to_upload(d.can_choose_couple_to_upload)
	, multipart_content_length_threshold(d.multipart_content_length_threshold)
{
}

namespace_settings_t::data::data(namespace_settings_t::data &&d)
	: name(std::move(d.name))
	, groups_count(std::move(d.groups_count))
	, success_copies_num(std::move(d.success_copies_num))
	, auth_key(std::move(d.auth_key))
	, static_couple(std::move(d.static_couple))
	, auth_key_for_write(std::move(d.auth_key_for_write))
	, auth_key_for_read(std::move(d.auth_key_for_read))
	, sign_token(std::move(d.sign_token))
	, sign_path_prefix(std::move(d.sign_path_prefix))
	, sign_port(std::move(d.sign_port))
	, redirect_expire_time(d.redirect_expire_time)
	, redirect_content_length_threshold(d.redirect_content_length_threshold)
	, is_active(d.is_active)
	, can_choose_couple_to_upload(d.can_choose_couple_to_upload)
	, multipart_content_length_threshold(d.multipart_content_length_threshold)
{
}

namespace_settings_t::namespace_settings_t() {
}

namespace_settings_t::namespace_settings_t(const namespace_settings_t &ns)
	: m_data(new data(*ns.m_data))
{
}
namespace_settings_t::namespace_settings_t(namespace_settings_t &&ns)
	: m_data(std::move(ns.m_data))
{
}

namespace_settings_t::namespace_settings_t(data &&d)
	: m_data(new data(std::move(d)))
{
}

namespace_settings_t &namespace_settings_t::operator =(namespace_settings_t &&ns) {
	m_data = std::move(ns.m_data);
	return *this;
}

namespace_settings_t::~namespace_settings_t() {
}

const std::string &namespace_settings_t::name() const {
	return m_data->name;
}

int namespace_settings_t::groups_count() const {
	return m_data->groups_count;
}

const std::string &namespace_settings_t::success_copies_num () const {
	return m_data->success_copies_num;
}

const std::string &namespace_settings_t::auth_key () const {
	return m_data->auth_key.empty() ? m_data->auth_key_for_write : m_data->auth_key;
}

const std::vector<int> &namespace_settings_t::static_couple () const {
	return m_data->static_couple;
}

const std::string &namespace_settings_t::sign_token () const {
	return m_data->sign_token;
}

const std::string &namespace_settings_t::sign_path_prefix () const {
	return m_data->sign_path_prefix;
}

const std::string &namespace_settings_t::sign_port () const {
	return m_data->sign_port;
}

const std::string &namespace_settings_t::auth_key_for_write() const {
	return m_data->auth_key_for_write.empty() ? m_data->auth_key : m_data->auth_key_for_write;
}

const std::string &namespace_settings_t::auth_key_for_read() const {
	return m_data->auth_key_for_read;
}

int namespace_settings_t::redirect_expire_time() const {
	return m_data->redirect_expire_time;
}

int64_t namespace_settings_t::redirect_content_length_threshold() const {
	return m_data->redirect_content_length_threshold;
}

bool namespace_settings_t::is_active() const {
	return m_data->is_active;
}

bool namespace_settings_t::can_choose_couple_to_upload() const {
	return m_data->can_choose_couple_to_upload;
}

int64_t namespace_settings_t::multipart_content_length_threshold() const {
	return m_data->multipart_content_length_threshold;
}

} //mastermind

