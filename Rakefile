require 'bundler/gem_tasks'
require 'rake/extensiontask'
require 'rake/testtask'

task :default => [:compile, :test]

Rake::ExtensionTask.new('ext') do |ext|
	ext.ext_dir = 'ext/dvb'
	ext.lib_dir = 'lib/dvb'
end

Rake::TestTask.new {|t| t.libs << 'test'}
