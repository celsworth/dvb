# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'dvb/version'

Gem::Specification.new do |spec|
	spec.name          = "dvb"
	spec.version       = DVB::VERSION
	spec.authors       = ["Chris Elsworth"]
	spec.email         = ["chris@shagged.org"]
	spec.summary       = ""
	spec.homepage      = ""
	spec.license       = "MIT"

	#spec.files         = `git ls-files -z`.split("\x0")
	spec.files          = Dir['lib/**/*.rb'] + Dir['bin/*'] + Dir['ext/**/*.c'] + Dir['ext/**/extconf.rb']
	spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
	spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
	spec.require_paths = ['lib', 'ext']
	spec.extensions    = Dir['ext/**/extconf.rb']

	spec.add_development_dependency "bundler"
	spec.add_development_dependency "reek"
	spec.add_development_dependency "rake"
	spec.add_development_dependency "rake-compiler"


end
