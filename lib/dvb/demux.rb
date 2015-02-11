module DVB
	class Demux
		attr_reader :adapter, :demux

		def initialize(adapter, demux, opts = {})
			@adapter, @demux = adapter, demux

		end

	end
end
