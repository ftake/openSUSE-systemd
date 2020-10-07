/* SPDX-License-Identifier: LGPL-2.1+ */
#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#include "sd-netlink.h"

#include "conf-parser.h"
#include "in-addr-util.h"
#include "networkd-link.h"
#include "networkd-util.h"

typedef struct Manager Manager;
typedef struct Network Network;

/* See struct rtvia in rtnetlink.h */
typedef struct RouteVia {
        uint16_t family;
        union in_addr_union address;
} _packed_ RouteVia;

typedef struct MultipathRoute {
        RouteVia gateway;
        int ifindex;
        uint32_t weight;
} MultipathRoute;

typedef struct Route {
        Network *network;
        NetworkConfigSection *section;

        Link *link;
        Manager *manager;

        int family;
        int gw_family;
        int quickack;
        int fast_open_no_cookie;
        int ttl_propagate;

        unsigned char dst_prefixlen;
        unsigned char src_prefixlen;
        unsigned char scope;
        bool scope_set;
        unsigned char protocol;  /* RTPROT_* */
        unsigned char type; /* RTN_* */
        unsigned char tos;
        uint32_t priority; /* note that ip(8) calls this 'metric' */
        uint32_t table;
        bool table_set;
        uint32_t mtu;
        uint32_t initcwnd;
        uint32_t initrwnd;
        unsigned char pref;
        unsigned flags;
        int gateway_onlink;
        bool gateway_from_dhcp;

        union in_addr_union gw;
        union in_addr_union dst;
        union in_addr_union src;
        union in_addr_union prefsrc;
        OrderedSet *multipath_routes;

        usec_t lifetime;
        sd_event_source *expire;
} Route;

void route_hash_func(const Route *route, struct siphash *state);
int route_compare_func(const Route *a, const Route *b);
extern const struct hash_ops route_hash_ops;

int route_new(Route **ret);
Route *route_free(Route *route);
DEFINE_NETWORK_SECTION_FUNCTIONS(Route, route_free);

int route_configure(Route *route, Link *link, link_netlink_message_handler_t callback, Route **ret);
int route_remove(Route *route, Manager *manager, Link *link, link_netlink_message_handler_t callback);

int link_set_routes(Link *link);
int link_drop_routes(Link *link);
int link_drop_foreign_routes(Link *link);
int link_serialize_routes(Link *link, FILE *f);
int link_deserialize_routes(Link *link, const char *routes);

uint32_t link_get_dhcp_route_table(Link *link);
uint32_t link_get_ipv6_accept_ra_route_table(Link *link);

int manager_rtnl_process_route(sd_netlink *rtnl, sd_netlink_message *message, Manager *m);

int network_add_ipv4ll_route(Network *network);
int network_add_default_route_on_device(Network *network);
void network_drop_invalid_routes(Network *network);

CONFIG_PARSER_PROTOTYPE(config_parse_gateway);
CONFIG_PARSER_PROTOTYPE(config_parse_preferred_src);
CONFIG_PARSER_PROTOTYPE(config_parse_destination);
CONFIG_PARSER_PROTOTYPE(config_parse_route_priority);
CONFIG_PARSER_PROTOTYPE(config_parse_route_scope);
CONFIG_PARSER_PROTOTYPE(config_parse_route_table);
CONFIG_PARSER_PROTOTYPE(config_parse_route_boolean);
CONFIG_PARSER_PROTOTYPE(config_parse_ipv6_route_preference);
CONFIG_PARSER_PROTOTYPE(config_parse_route_protocol);
CONFIG_PARSER_PROTOTYPE(config_parse_route_type);
CONFIG_PARSER_PROTOTYPE(config_parse_tcp_window);
CONFIG_PARSER_PROTOTYPE(config_parse_route_mtu);
CONFIG_PARSER_PROTOTYPE(config_parse_multipath_route);
