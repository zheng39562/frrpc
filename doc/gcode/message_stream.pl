#! /usr/bin/perl

##########################################################################################
## perl 一些语法
# \ 取引用

##########################################################################################
# 消息流
#
use Graph::Easy;
use File::Basename;

do "./public.pl";
do "./gword.pl";

my $graph = Graph::Easy->new();
my $route_group = $graph->add_group("route connection");
my $client_edge_r = $route_group->add_edge("client", "route");
my $route_edge_r = $route_group->add_edge("route", "server");
$client_edge_r->set_attribute("label", "message");
$route_edge_r->set_attribute("label", "message");

my $direct_group = $graph->add_group("direct connection");
my $client_edge_d = $direct_group->add_edge("client", "route");
my $route_edge_d = $direct_group->add_edge("route", "server");
$client_edge_d->set_attribute("label", "message");
$route_edge_d->set_attribute("label", "message");

## 储存
my $out_path = "../graph/";
my $file_name = __FILE__;
$file_name =~ s/.pl//;
open($file, ">$out_path$file_name.txt");
print $file ($graph->as_ascii());
close($file);

