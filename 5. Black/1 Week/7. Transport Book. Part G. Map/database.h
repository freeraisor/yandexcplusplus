//
// Created by ilya on 25.11.2019.
//
#ifndef YANDEXCPLUSPLUS_4_BROWN_FINAL_PROJECT_PART_A_DATABASE_H
#define YANDEXCPLUSPLUS_4_BROWN_FINAL_PROJECT_PART_A_DATABASE_H
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <memory>
#include <list>

#include "graph.h"
#include "dijkstra.h"
#include "json.h"
#include "coordinates.h"

namespace TransportDatabase {
class Stop {
public:

  Stop();
  Stop(std::string name, const Coordinates &coord);
  Stop(std::string name, const Coordinates &coord, const std::vector<std::pair<std::string, int>> &distances);
  Stop(const Stop &other);
  Stop(Stop &&other) noexcept;
  Stop &operator=(const Stop &other);
  Stop &operator=(Stop &&other) noexcept;

  std::string GetName() const;
  Coordinates GetCoord() const;

  void AddRoute(const std::string &route_name);
  std::vector<std::string> TakeRoutes() const;

  std::unordered_map<std::string, int> distance_to_stop;
private:
  std::string name_;
  Coordinates coord_;
  std::set<std::string> routes_for_stop;
};

class Route {
public:
  enum class RouteTypes { LINEAR, CYCLE };
  explicit Route(const std::string &name, const std::vector<std::string> &stops_name, RouteTypes type)
      : route_type(type), name_(name), stops_name_(stops_name) {}
  virtual ~Route() = default;
  virtual size_t CountOfStops() const = 0;
  virtual size_t CountOfUniqueStops() const = 0;
  virtual double RealLength() const = 0;
  virtual double Length() const = 0;
  double Curvature() const;
  std::string GetName() const;
  std::vector<std::string> GetStopsName() const;

  RouteTypes route_type;
protected:
  std::string name_;
  std::vector<std::string> stops_name_;
};

static const std::unordered_map<char, Route::RouteTypes> sign_to_route = {{'-', Route::RouteTypes::LINEAR},
                                                                          {'>', Route::RouteTypes::CYCLE}};

class LinearRoute : public Route {
public:
  LinearRoute() = default;
  explicit LinearRoute(const std::string &name, const std::vector<std::string> &stops_name,
                       const std::vector<std::shared_ptr<Stop>> &stops) : Route(name, stops_name, RouteTypes::LINEAR),
                                                                          stops_(stops) {
    for (const auto &elem : stops_name_) {
      unique_stops_.insert(elem);
    }
  }
  size_t CountOfStops() const override;
  size_t CountOfUniqueStops() const override;
  double RealLength() const override;
  double Length() const override;
private:
  // Порядок маршрутов с данными о остановках
  std::vector<std::shared_ptr<Stop>> stops_;
  // Хранение уникальных остановок
  std::unordered_set<std::string_view> unique_stops_;
};

class CycleRoute : public Route {
public:
  CycleRoute() = default;
  explicit CycleRoute(const std::string &name, const std::vector<std::string> &stops_name,
                      const std::vector<std::shared_ptr<Stop>> &stops) : Route(name, stops_name, RouteTypes::CYCLE),
                                                                         stops_(stops) {
    for (const auto &elem : stops_name_) {
      unique_stops_.insert(elem);
    }
  }
  size_t CountOfStops() const override;
  size_t CountOfUniqueStops() const override;
  double RealLength() const override;
  double Length() const override;
private:
  // Порядок маршрутов с данными о остановках
  std::vector<std::shared_ptr<Stop>> stops_;
  // Хранение уникальных остановок
  std::unordered_set<std::string_view> unique_stops_;
};

struct RouteInfo {
  Route::RouteTypes type;
  std::string name;
  std::vector<std::string> stop_names;
};

struct DatabaseStat {
  double min_lat = std::numeric_limits<double>::max();
  double max_lat = 0;
  double min_long = std::numeric_limits<double>::max();
  double max_long = 0;
};

class Database {
public:
  using StopData = std::unordered_map<std::string, std::shared_ptr<Stop>>;
  using RouteData = std::unordered_map<std::string, std::shared_ptr<Route>>;
  Database() = default;
  void AddStop(const Stop &stop);
  std::shared_ptr<Stop> TakeOrAddStop(const std::string &stop_name);
  std::shared_ptr<Stop> TakeStop(const std::string &stop_name) const;
  const StopData& TakeStops() const;

  void AddRoute(const std::string &route_name, std::shared_ptr<Route> route);
  std::shared_ptr<Route> TakeRoute(const std::string &route_name) const;
  // TODO возвращать контейнер, который НЕ позволяет изменять хранимые значения?
  const RouteData& TakeRoutes() const;

  const DatabaseStat& TakeStat() const;
private:
  DatabaseStat stat;
  StopData stops_;
  RouteData routes_;
};

class RouteBuilder {
public:
  RouteBuilder() = delete;
  explicit RouteBuilder(Database &db) : db_(db) {}
  std::shared_ptr<Route> MakeRoute(RouteInfo &&info);
private:
  Database &db_;
  std::shared_ptr<Route> MakeCycle(RouteInfo &&info);
  std::shared_ptr<Route> MakeLinear(RouteInfo &&info);
};
}
#endif //YANDEXCPLUSPLUS_4_BROWN_FINAL_PROJECT_PART_A_DATABASE_H
