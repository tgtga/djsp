#!/usr/bin/env ruby

require "ffi"

module DJSP
  module C
    extend FFI::Library
    ffi_lib "c"
    ffi_lib "./build/libdjsp.so"

    # step

    [
      :step_realloc_before_up,   :step_realloc_after_up,
      :step_realloc_before_down, :step_realloc_after_down
    ].each {|name| attach_variable name, :bool }

    # logging
    
    attach_function :setup, [], :void
    attach_function :set_log, [:string], :void
    attach_function :djsp_message, [:string, :varargs], :void

    # oneshot

    attach_variable :show_steps, :bool
    attach_variable :ssol, :size_t

    # callback :memo_callback, [:uint64, :uint64, :pointer], :uint64
    attach_function :oneshot_2, [:uint64], :uint64
    attach_function :oneshot_n, [:uint64, :uint64], :uint64

    # sequence

		callback :memo_read_callback, [:uint64], :uint64
		callback :memo_write_callback, [*[:uint64] * 2], :void
    callback :hwm_callback, [*[:uint64] * 3], :void
    attach_function :sequence,           [:uint64, :uint64, :uint64, :memo_read_callback, :memo_write_callback, :hwm_callback], :void
    attach_function :sequence_2_rootopt, [         :uint64, :uint64, :memo_read_callback, :memo_write_callback, :hwm_callback], :void
    attach_function :sequence_2_p,       [         :uint64, :uint64, :memo_read_callback, :memo_write_callback, :hwm_callback], :void
  end

  public

  VERSION_MAJOR = 1
  VERSION_MINOR = 0
  VERSION_PATCH = 0
  VERSION       = "1.0.0"

  VALID_SEQUENCE_OPTIMIZATIONS = [:root, :none]

  class << self
    def ssol; C::ssol; end
    def ssol= x
      if x.nil?
        C::ssol = 0
        C::show_steps = false
      end

      raise ArgumentError unless x.is_a? Integer

      C::ssol = x
      C::show_steps = true
    end
    alias show_steps_over_log ssol
    alias show_steps_over_log= ssol=

    @log = nil
    def log; @log; end
    def log= file
      # clear set log file
      if file.nil?
        @log = nil
        C::set_log nil
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

    def message what
      time = Time.now.strftime "%FT%T.%L%z"
      puts "#{time}: #{what}"
      @log&.puts "#{time}: #{what}"
    end

    def oneshot seed, base: nil, &memo
      case base
      when nil then C::oneshot_2 seed
      else          C::oneshot_n seed, base
      end
    end

    def sequence range = (1..), base: nil, optimize: nil, alert: nil, &hwm
      unless range.is_a? Enumerator::ArithmeticSequence
      	begin
      		range = range.step
      		raise unless range.is_a? Enumerator::ArithmeticSequence
      	rescue
        	raise TypeError, "`range` must be or be able to be converted to an Enumerator::ArithmeticSequence"
        end
      end

      left = range.begin || 1
      (right, endless) = range.end ? [range.end, false] : [0, true]
      step = range.step

      base ||= 0

			unless optimize.nil?
		    optimize = optimize.to_sym
		    raise ArgumentError, "the only valid optimizations are: #{VALID_SEQUENCE_OPTIMIZATIONS.join ", "}" \
		      unless VALID_SEQUENCE_OPTIMIZATIONS.include? optimize
			end

      if block_given?
      	hwm = FFI::Function.new :void, [*[:uint64] * 3], hwm
      else
        out = {}
        hwm = FFI::Function.new(:void, [*[:uint64] * 3]) do |index, mark, where|
        	STDERR.puts "ruby entering function (#{index}, #{mark}, #{where})"
        	out[where] = mark
        	STDERR.puts "ruby exiting function (#{index}, #{mark}, #{where})"
        end
      end

      if optimize == :root
        raise RangeError, "root optimization not implemented for bases other than 2" \
          if base > 2

				STDERR.puts "sequence_2_rootopt with [#{left}, #{right}, nil, nil, #{hwm}]"
        C::sequence_2_rootopt left, right, nil, nil, hwm
      elsif optimize == :none || base > 2
				STDERR.puts "sequence with [#{base}, #{left}, #{right}, nil, nil, #{hwm}]"
        C::sequence base, left, right, nil, nil, hwm
      else
				STDERR.puts "sequence_2_p with [#{left}, #{right}, nil, nil, #{hwm}]"
				C::sequence_2_p left, right, nil, nil, hwm
      end

      out unless block_given?
    end

    def equal_to(what, base: nil) = (1..).lazy.filter { oneshot(_1, base: base) == what } 
    def first_equal_to(what, base: nil) = equal_to(what, base: base).first
    def equal_to_over range, base: nil, all: false
      memo = {}

      range.each do |i|
        res = oneshot i, base: base
        
        if all
          memo[res] ||= []
          memo[res] << i
        else
          memo[res] ||= i
        end
      end

      memo.sort_by {|k, _| k }.to_h
    end
  end

  C::setup
  freeze
end

if $0 == __FILE__
  require "optparse"

  options = {}
  OptionParser.new do |parser|
    parser.banner = "usage: #$0 [options]"

    parser.on_head(
      "-h", "--help",
      "show this help"
    ) { puts parser; exit }

    parser.on(
      "-l LOG", "--log LOG",
      "tee all output to LOG"
    ) {|log| DJSP.log = log }

    parser.on(
      "-s [BL]", "--steps [BL]", Integer,
      "show intermediate steps for values which are greater than this (B)ase-2 (L)ogarithm"
    ) {|steps| DJSP.show_steps_over_log = steps || 0 }

    parser.on(
      "-b [BASE = 2]", "--base [BASE = 2]", Integer,
      "set the base used for calculations to BASE"
    ) {|base| options[:base] = base || 2 }

    parser.on(
      "-a [STEP = 1]", "--alert-every [STEP = 1]", Integer,
      "alert when the sequence has elapsed STEP values"
    ) {|alert| options[:alert] = alert || 1 }
    
    [
      [?u, "before", "up"  ],
      [?U, "after",  "up"  ],
      [?d, "before", "down"],
      [?D, "after",  "down"]
    ].each do |(flag, where, step)|
      parser.on(
        "-#{flag}", "--#{where}-#{step}", TrueClass,
        "run bignum reallocations #{where} the #{step} step"
      ) { DJSP::C.send :"step_realloc_#{where}_#{step}=", true }
    end
  end.parse!

  raise OptionParser::InvalidOption, ARGV[1] if ARGV.length > 1
  
  if ARGV.length == 1 
    options[:over] =
      begin
        Integer ARGV.first
      rescue ArgumentError
        ARGV.first =~ /(\d*)\.\.(\d*)(?:,(\d+))?/
        raise OptionParser::InvalidOption, ARGV.first unless $~

        (start, ending) = [$1, $2].map { _1.empty? ? nil : _1.to_i }
        range = (start..ending)
        range = range.step $3.to_i if $3
        range
      end
  end

  options[:over] ||= (1..)

  if options[:over].is_a? Enumerable
    DJSP.sequence options[:over], base: options[:base], alert: options[:alert] do |index, mark, where|
      DJSP.message "A#{options[:base] ? "_#{options[:base]}" : ""}(#{index}) @ #{where} = #{mark}"
    end
  else
    r = DJSP.oneshot options[:over], base: options[:base]
    DJSP.message "J#{options[:base] ? "_#{options[:base]}" : ""}(#{options[:over]}) = #{r}"
  end
end
