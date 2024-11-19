#!/usr/bin/ruby

require 'readline'
require 'parslet'

def main
  loop do
    cmdline = Readline.readline(">> ", true)
    tree = parse_cmdline(cmdline)
    tree.execute
  end
end

def parse_cmdline(cmdline)
  raw_tree = Parser.new.parse(cmdline)
  p raw_tree
  Transform.new.apply(raw_tree)
end

class Parser < Parslet::Parser
  root :cmdline

  rule(:cmdline) { command }
  rule(:command) { arg.as(:arg).repeat(1).as(:command) }
  rule(:arg) { match[%q{^\s}].repeat(1) >> space? }

  rule(:space?) { space.maybe }
  rule(:space) { match[%q{\s}].repeat(1).ignore }
end

class Transform < Parslet::Transform
  rule(command: sequence(:args)) { Command.new(args) }
  rule(arg: simple(:arg)) { arg }
end

class Command
  def initialize(args)
    @args = args
  end

  def execute
    spawn(*@args)
  end
end

main
