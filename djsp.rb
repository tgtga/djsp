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
    
    attach_function :set_up_log, [:string], :void
    attach_function :djsp_message, [:string, :varargs], :void

    # oneshot

    attach_variable :show_steps, :bool
    attach_variable :ssol, :size_t

    # callback :memo_callback, [:uint64, :uint64, :pointer], :uint64
    attach_function :oneshot_2, [:uint64], :uint64
    attach_function :oneshot_n, [:uint64, :uint64], :uint64

    # sequence

    callback :hwm_callback, [*[:uint64] * 3], :void
    attach_function :sequence, [:uint64, *[:uint64] * 3, :bool, :hwm_callback], :void
    attach_function :sequence_alert, [:uint64, *[:uint64] * 3, :bool, :uint64, :hwm_callback], :void
    attach_function :sequence_rootopt, [*[:uint64] * 3, :bool, :hwm_callback], :void
  end

  public

  VERSION_MAJOR = 1
  VERSION_MINOR = 0
  VERSION_PATCH = 0
  VERSION       = "1.0.0"

  VALID_SEQUENCE_OPTIMIZATIONS = [:root]

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

    def sequence range = (1..), base: nil, optimize: [], alert: nil, &hwm
      unless range.is_a? Enumerator::ArithmeticSequence
        range = range.step
        raise TypeError, "`range` must be or be able to be converted to an Enumerator::ArithmeticSequence" \
          unless range.is_a? Enumerator::ArithmeticSequence
      end

      left = range.begin || 1
      (right, endless) = range.end ? [range.end, false] : [0, true]
      step = range.step

      base ||= 0

      optimize = [optimize] if optimize.is_a?(Symbol) || optimize.is_a?(String)
      optimize = optimize.map &:to_sym
      raise ArgumentError, "the only valid optimizations are: #{VALID_SEQUENCE_OPTIMIZATIONS.join ?,}" \
        unless (optimize - VALID_SEQUENCE_OPTIMIZATIONS).empty?

      unless block_given?
        out = {}
        hwm = ->(index, mark, where){ out[where] = mark }
      end

      if optimize.include? :root
        raise RangeError, "root optimization not implemented for bases other than 2" \
          if base > 2

        C::sequence_rootopt left, right, step, endless, hwm
      elsif alert
        C::sequence_alert base, left, right, step, endless, alert, hwm
      else
        C::sequence base, left, right, step, endless, hwm
      end

      out unless block_given?
    end
  end
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
