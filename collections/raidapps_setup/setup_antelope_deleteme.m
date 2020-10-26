% SETUP_ANTELOPE Set up the Matlab path for Antelope
%
%
%	SETUP_ANTELOPE
%
%
%	SETUP_ANTELOPE adds the Antelope paths for tools, examples, and 
%	scripts to the Matlab path. 
%
%	Antelope Toolbox for Matlab
%	   [Antelope is a product of Boulder Real-Time Technologies, Inc.]
%	Kent Lindquist
%	Lindquist Consulting
%	1997-2010

ATM_prefix = [getenv( 'ANTELOPE' ), '/local/data/matlab/', version( '-release' ), '/antelope/'];

% Sun JVM throws and traps SIGILL and SIGSEGV, which must be removed 
% from the default ELOG_SIGNALS mask to prevent elog_fault(3) crashes 
% of the Matlab interpreter while running 64-bit Antelope Toolbox for Matlab
% under Sun Solaris:

setenv( 'ELOG_SIGNALS', 'TRAP:FPE:BUS:SYS' );

if isempty( getenv( 'ANTELOPE' ) ) 

	display('Please set the ANTELOPE environment variable before running setup_antelope.m' );

else

	addpath( [ATM_prefix , 'antelope'] );
	addpath( [ATM_prefix , 'scripts'] );
	addpath( [ATM_prefix , 'examples'] );
	addpath( [ATM_prefix , 'user'] );

end
	
clear ATM_prefix;

