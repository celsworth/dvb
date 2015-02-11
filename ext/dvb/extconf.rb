require 'mkmf'

RbConfig::MAKEFILE_CONFIG['CC'] = ENV['CC'] if ENV['CC']

unless pkg_config('libdvbv5')
	raise 'libdvbv5 not found, try apt-get install libdvbv5-dev'
end

unless have_func 'dvb_fe_open_flags', 'libdvbv5/dvb-fe.h'
	raise 'dvb_fe_open_flags not found?!'
end

create_header
create_makefile('ext')
