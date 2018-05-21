#! /usr/bin/perl

##########################################################################################
# 主体设想
#
use Graph::Easy;
use File::Basename;

my $out_path = "../graph/";
my $file_name = __FILE__;
$file_name =~ s/.pl//;

my $graph = Graph::Easy->new();

$graph->add_edge("中文", "b");
$graph->add_node("c");
my $group = $graph->add_group("d");
$group->add_edge("g", "h");
$group->add_member("e");
$group->add_node("f");

open($file, ">$out_path$file_name.txt");
print $file ($graph->as_ascii());
close($file);

