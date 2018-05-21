#! /usr/bin/perl

package public;

sub T_return(){ return "return 1"; }



sub WriteFile(){
	my $file_path = $_[0];
	my $content = $_[1];
	open($file, ">$file_path");
	print $file ($graph->as_ascii());
	close($file);
}

__END__

