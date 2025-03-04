require "ffi"

module DJSP
  module C
    extend FFI::Library
    ffi_lib "c"
    ffi_lib "./build/libdjsp.so"

    # logging
    
    attach_function :set_up_log, [:string], :void
    attach_function :message, [:string, :varargs], :void

    # oneshot

    attach_variable :show_steps, :bool
    attach_variable :ssol, :size_t

    callback :memo_callback, [:uint64, :uint64, :pointer], :uint64
    attach_function :oneshot_2, [:uint64, :memo_callback], :uint64
    attach_function :oneshot_n, [:uint64, :uint64, :memo_callback], :uint64

    # sequence

    callback :hwm_callback, [*[:uint64] * 3], :void
    attach_function :sequence, [:uint64, *[:uint64] * 3, :bool, :memo_callback, :hwm_callback], :void
    attach_function :sequence_rootopt, [*[:uint64] * 3, :bool, :memo_callback, :hwm_callback], :void
  end

  public

  VALID_SEQUENCE_OPTIMIZATIONS = [:root]

  class << self
    # def show_steps; C::show_steps; end
    # def show_steps= x
    #   raise ArgumentError unless x == true || x == false
    #   C::show_steps = x
    # end
    # 
    # def ssol; C::ssol; end
    # def ssol= x
    #   raise ArgumentError unless x.is_a? Integer
    #   C::ssol = x
    # end
    # alias show_steps_over_log ssol
    # alias show_steps_over_log= ssol=

    @log = nil
    def log; @log; end
    def log= file
      # clear set log file
      if file.nil?
        @log = nil
        C::set_up_log nil
        return
      end

      path =
        case file
        when File then file.to_path
        when String then file
        else raise ArgumentError, "the log file must be a File or a path to open"
        end

      # open file, ensure that ruby doesn't buffer any writes
      # (try this with sync = true and sync = false):
      # file.write "i <3 sweet exorcist's testone"
      # `kill -9 $(pgrep ruby)`
      # file.write "and testtwo\n"
      @log = File.new(path, "a").tap { _1.sync = true }

      C::set_up_log path
    end

    def time_format; C::time_format; end
    def time_format= time_format; C::time_format = time_format; end

    def oneshot seed, base: nil, &memo
      case base
      when nil then C::oneshot_2       seed, memo
      else          C::oneshot_n seed, base, memo
      end
    end

    def sequence range = (1..), base: 0, optimize: [], &hwm
      unless range.is_a? Enumerator::ArithmeticSequence
        range = range.step
        raise TypeError, "`range` must be or be able to be converted to an Enumerator::ArithmeticSequence" \
          unless range.is_a? Enumerator::ArithmeticSequence
      end

      left = range.begin || 1
      (right, endless) = range.end ? [range.end, false] : [0, true]
      step = range.step

      optimize = [optimize] if optimize.is_a?(Symbol) || optimize.is_a?(String)
      optimize = optimize.map &:to_sym
      raise ArgumentError, "the only valid optimizations are: #{VALID_SEQUENCE_OPTIMIZATIONS.join ?,}" \
        unless optimize.-(VALID_SEQUENCE_OPTIMIZATIONS).empty?

      unless block_given?
        out = {}
        hwm = ->(index, mark, where){ out[where] = mark }
      end

      if optimize.include? :root
        raise RangeError, "root optimization not implemented for bases other than 2" \
          if base > 2

        C::sequence_rootopt left, right, step, endless, nil, hwm
      else
        C::sequence base, left, right, step, endless, nil, hwm
      end

      out unless block_given?
    end
  end
end

if $0 == __FILE__
  require 'optparse'

  USAGE = <<~HERE
    usage: #{$0} [options] [range] [base]

    -h --help          show this help

    -l --log           log all information to a file
    -s --steps         show intermediate steps for values
                       whose base-2 logarithm exceeds this value
    
    -u --before-up     run bignum reallocations before the up step
    -U --after-up      run bignum reallocations after the up step
    -d --before-down   run bignum reallocations before the down step
    -D --after-down    run bignum reallocations after the down step
  HERE

  OptionParser.new do |parser|
    parser.banner = "usage: #$0 [options]"

    parser.on(
      "-lLOG", "--log=LOG",
      "tee all output to LOG"
    ) {|log| DJSP.log = log }

    parser.on(
      
    )

    parser.on(
      "-h", "--help",
      "show this help"
    ) { puts parser; exit }
  end.parse!
end
