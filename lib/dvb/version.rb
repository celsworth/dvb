module DVB
	# Major version
	MAJOR = 0

	# Minor version
	MINOR = 0

	# Tiny version
	TINY = 1

	# Version string
	VERSION = [MAJOR, MINOR, TINY].join('.')

	# Returns the version string for the library.
	def self.version
		VERSION
	end
end
