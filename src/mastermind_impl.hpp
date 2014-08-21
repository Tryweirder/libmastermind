#ifndef SRC__MASTERMIND_IMPL_HPP
#define SRC__MASTERMIND_IMPL_HPP

#include "libmastermind/mastermind.hpp"
#include "utils.hpp"

#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <fstream>
#include <functional>
#include <utility>

#include <cocaine/framework/service.hpp>
#include <cocaine/framework/services/app.hpp>
#include <cocaine/framework/common.hpp>
#include <cocaine/traits/tuple.hpp>

#include <boost/lexical_cast.hpp>

namespace mastermind {

struct mastermind_t::data {
	data(const remotes_t &remotes, const std::shared_ptr<cocaine::framework::logger_t> &logger,
			int group_info_update_period, std::string cache_path, int expired_time,
			std::string worker_name);
	~data();

	void stop();

	void reconnect();

	template <typename R, typename T>
	bool simple_enqueue(const std::string &event, const T &chunk, R &result);

	template <typename R, typename T>
	void enqueue(const std::string &event, const T &chunk, R &result);

	bool collect_group_weights();
	bool collect_bad_groups();
	bool collect_symmetric_groups();
	bool collect_cache_groups();
	bool collect_namespaces_settings();
	bool collect_metabalancer_info();
	bool collect_namespaces_statistics();
	bool collect_elliptics_remotes();

	void collect_info_loop_impl();
	void collect_info_loop();

	void serialize();
	void deserialize(int expired_time);

	void cache_force_update();
	void set_update_cache_callback(const std::function<void (void)> &callback);

	std::shared_ptr<cocaine::framework::logger_t> m_logger;

	remotes_t                                          m_remotes;
	remote_t                                           m_current_remote;
	size_t                                             m_next_remote;
	std::string                                        m_cache_path;
	std::string                                        m_worker_name;

	int                                                m_metabase_timeout;
	uint64_t                                           m_metabase_current_stamp;

	template <typename T>
	struct cache_t {
		typedef T cache_type;
		typedef std::shared_ptr<cache_type> cache_ptr_type;
		cache_ptr_type cache;
		std::recursive_mutex mutex;

		cache_t() {
			cache = std::make_shared<cache_type>();
		}

		template <typename... Args>
		static cache_ptr_type create(Args&&... args) {
			return std::make_shared<cache_type>(std::forward<Args>(args)...);
		}

		void swap(cache_ptr_type &ob) {
			std::lock_guard<std::recursive_mutex> lock(mutex);
			cache.swap(ob);
		}

		cache_ptr_type copy() {
			std::lock_guard<std::recursive_mutex> lock(mutex);
			return cache;
		}
	};

	cache_t<std::vector<std::vector<int>>> m_bad_groups;
	cache_t<std::map<int, std::vector<int>>> m_symmetric_groups;
	cache_t<metabalancer_groups_info_t> m_metabalancer_groups_info;
	cache_t<std::vector<namespace_settings_t>> m_namespaces_settings;
	cache_t<std::map<std::string, std::vector<int>>> m_cache_groups;
	cache_t<metabalancer_info_t> m_metabalancer_info;
	cache_t<namespaces_statistics_t> m_namespaces_statistics;
	cache_t<std::vector<std::string>> m_elliptics_remotes;

	const int                                          m_group_info_update_period;
	std::thread                                        m_weight_cache_update_thread;
	std::condition_variable                            m_weight_cache_condition_variable;
	std::mutex                                         m_mutex;
	std::function<void (void)>                         m_cache_update_callback;
	bool                                               m_done;
	std::mutex                                         m_reconnect_mutex;

	std::shared_ptr<cocaine::framework::app_service_t> m_app;
	std::shared_ptr<cocaine::framework::service_manager_t> m_service_manager;
};

template <typename R, typename T>
bool mastermind_t::data::simple_enqueue(const std::string &event, const T &chunk, R &result) {
	try {
		auto g = m_app->enqueue(event, chunk);
		g.wait_for(std::chrono::seconds(4));

		if (g.ready() == false) {
			return false;
		}

		auto chunk = g.next();

		result = cocaine::framework::unpack<R>(chunk);
		return true;
	} catch (const std::exception &ex) {
		COCAINE_LOG_ERROR(m_logger, "libmastermind: enqueue_impl: %s", ex.what());
	}
	return false;
}

template <typename R, typename T>
void mastermind_t::data::enqueue(const std::string &event, const T &chunk, R &result) {
	bool tried_to_reconnect = false;
	try {
		if (!m_service_manager || !m_app || m_app->status() != cocaine::framework::service_status::connected) {
			COCAINE_LOG_INFO(m_logger, "libmastermind: enqueue: preconnect");
			tried_to_reconnect = true;
			reconnect();
		}

		if (simple_enqueue(event, chunk, result)) {
			return;
		}

		if (tried_to_reconnect) {
			throw std::runtime_error("cannot process enqueue");
		}

		reconnect();

		if (simple_enqueue(event, chunk, result)) {
			return;
		}

		throw std::runtime_error("cannot reprocess enqueue");
	} catch (const cocaine::framework::service_error_t &ex) {
		COCAINE_LOG_ERROR(m_logger, "libmastermind: enqueue: %s", ex.what());
		throw;
	} catch (const std::exception &ex) {
		COCAINE_LOG_ERROR(m_logger, "libmastermind: enqueue: %s", ex.what());
		throw;
	}
}

} // namespace mastermind

#endif /* SRC__MASTERMIND_IMPL_HPP */
