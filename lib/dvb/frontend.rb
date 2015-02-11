module DVB
	class Frontend
		attr_reader :adapter, :frontend, :verbose, :use_legacy_call

		def initialize(adapter, frontend, opts = {})
			@adapter, @frontend = adapter, frontend

			@verbose         = opts.fetch(:verbose,         false)
			@use_legacy_call = opts.fetch(:use_legacy_call, false)
		end

		# open() in C extension
		# close() in C extension
	end
end
